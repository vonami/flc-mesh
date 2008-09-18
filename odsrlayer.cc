/*
    Copyright (C) 2008    Dmitry Ivanov <vonami@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "odsrlayer.h"
#include "crisproutingcontroller.h"

#include <IPAddressResolver.h>
#include <IPv4InterfaceData.h>
#include <IPControlInfo.h>
#include <algorithm>

using namespace std;

Define_Module( OdsrLayer );

int OdsrLayer::numInitStages() const
{
    return 4;
}

void OdsrLayer::initialize( int stage )
{
    cSimpleModule::initialize( stage );

    if ( stage == 0 ) {
        // read the parameters
        m_odsrInterfaces = par( "odsrInterfaces" );
        m_autoassignAddressBase = IPAddress( par( "autoassignAddressBase" ).stringValue() );
        m_odsrPacketQueue = new OdsrPacketQueue( par( "packetQueueSize" ) );
        m_queueTimeout = par( "queueTimeout" );
        ev << "Queue timeout: " << m_queueTimeout << endl;
        m_odsrRoutingController = new CrispRoutingController();
    }

    if ( stage == 2 ) {

        cModule *host = parentModule();
        if ( !host )
            error( "No parent module found" );

        m_myAddress = IPAddress( m_autoassignAddressBase.getInt() + uint32( host->id() ) );
        ev << "Assigned address: " << m_myAddress << endl;

        // get standard IP routing table
        m_networkRoutingTable = IPAddressResolver().routingTableOf( host );
        if ( !m_networkRoutingTable )
            error( "No routing table found" );

        // get standard interface table
        InterfaceTable *interfaceTable = IPAddressResolver().interfaceTableOf( host );
        if ( !interfaceTable )
            error( "No interface table found" );

        // look at all interface table entries
        cStringTokenizer interfaceTokenizer( m_odsrInterfaces );
        const char *ifname;
        while ( ( ifname = interfaceTokenizer.nextToken() ) != NULL ) {
            InterfaceEntry* ie = interfaceTable->interfaceByName( ifname );
            if ( !ie )
                error( "No such interface" );

            // assign IP Address to all connected interfaces
            if ( !ie->isLoopback() ) {
                ie->ipv4()->setInetAddress( m_myAddress );
                ie->ipv4()->setNetmask( IPAddress::ALLONES_ADDRESS ); // set to ALLONES_ADDRESS to avoid auto-generation of routes

                // associate interface with default and LL_MANET_ROUTERS multicast groups
                IPv4InterfaceData::IPAddressVector mcg = ie->ipv4()->multicastGroups();
                if ( find( mcg.begin(), mcg.end(), IPAddress::ALL_HOSTS_MCAST ) == mcg.end() )
                    mcg.push_back( IPAddress::ALL_HOSTS_MCAST );
                if ( find(mcg.begin(), mcg.end(), IPAddress::ALL_ROUTERS_MCAST ) == mcg.end() )
                    mcg.push_back( IPAddress::ALL_ROUTERS_MCAST );
                if ( find(mcg.begin(), mcg.end(), LL_MANET_ROUTERS ) == mcg.end() )
                    mcg.push_back( LL_MANET_ROUTERS );

                ie->ipv4()->setMulticastGroups( mcg );
                ie->setBroadcast( true );
            }

            // add interface to LL_MANET_ROUTERS multicast group
            RoutingEntry* re = new RoutingEntry();
            re->host = LL_MANET_ROUTERS;
            re->netmask = IPAddress::ALLONES_ADDRESS;
            re->gateway = IPAddress();
            re->interfaceName = ifname;
            re->interfacePtr = ie;
            re->type = RoutingEntry::DIRECT;
            re->source = RoutingEntry::BGP;
            re->metric = 1;
            m_networkRoutingTable->addRoutingEntry( re );
        }
    }
}

void OdsrLayer::finish()
{
}

void OdsrLayer::handleMessage( cMessage* message )
{
    // process timeouts
    if ( message->isSelfMessage() ) {
        QueueTimeout *timeout = check_and_cast<QueueTimeout*>( message );
        processQueueTimeout( timeout );
        return;
    }

    const char *gate = message->arrivalGate()->name();
    ev << "Got message from gate: " << gate << endl;

    if ( strncmp( gate, "fromGenerator", 7 ) == 0 )
        handleOutgoingDataMessage( message );
    else if ( strncmp( gate, "fromNetwork", 11 ) == 0 )
        handleIncomingOdsrMessage( message );
    else
        error( "Do I have more gates?!" );
}

void OdsrLayer::handleOutgoingDataMessage( cMessage *message )
{
    // some debugging info
    const IPControlInfo *controlInfo = check_and_cast<IPControlInfo*>( message->controlInfo() );
    const IPAddress destination = controlInfo->destAddr();
    ev << "Outgoing data message, length: " << message->length() << ", destination: " << destination << endl;

    // what if the packet is for us?
    if ( destination == m_myAddress ) {
        error( "Got packet from generator which is for us!" );
    }

    // first enqueue the packet and then decide what to do with it
    m_odsrPacketQueue->push( message );

    // either dequeue this packet or issue a new route request
    if ( m_odsrRoutingTable.hasHopsFor( destination ) ) {
        dequeuePendingPackets( destination );
    }
    else {
        if ( m_pendingReplies.count( destination ) > 0 ) {
            ev << "A routing request underway" << endl;
            return;
        }

        // start a timer to bound route replies
        QueueTimeout *timeout = new QueueTimeout();
        timeout->setDestination( destination );
        scheduleAt( simTime() + m_queueTimeout, timeout );

        // initialize a new pending reply entry
        OdsrPendingReply preply;
        preply.seqnum = m_seqnum;
        m_pendingReplies[ destination ] = preply;

        // send a new request
        ODSRPacket *request = new ODSRPacket();
        request->setOdsrType( ODSR_REQUEST );
        request->setSource( m_myAddress );
        request->setDestination( destination );
        request->setSeqnum( m_seqnum++ );
        request->setPointer( 0 );
        sendToNetwork( request, LL_MANET_ROUTERS );
    }
}

void OdsrLayer::handleIncomingOdsrMessage( cMessage *message )
{
    ev << "Incoming IP/ODSR message, length: " << message->length() << endl;

    // get the IP control info block
    const IPControlInfo *control = check_and_cast<IPControlInfo*>( message->controlInfo() );
    if ( !control )
        error( "Why does the packet I got from IP have no control info attached?" );

    // create a new routing entry saying that this host is our neighbour
    ev << "IP packet source address: " << control->srcAddr() << endl;
    bool hasNeighbourEntry = false;
    for( int i = 0; i < m_networkRoutingTable->numRoutingEntries(); ++i ) {
        if ( m_networkRoutingTable->routingEntry( i )->host == control->srcAddr() &&
             m_networkRoutingTable->routingEntry( i )->gateway == control->srcAddr() ) {
            ev << "I already have an entry about this neighbour" << endl;
            hasNeighbourEntry = true;
            break;
        }
    }

    if ( !hasNeighbourEntry ) {
        RoutingEntry* re = new RoutingEntry();
        re->host = control->srcAddr();
        re->netmask = IPAddress::ALLONES_ADDRESS;
        re->gateway = control->srcAddr();
        re->type = RoutingEntry::DIRECT;
        re->source = RoutingEntry::BGP;
        re->metric = 1;

        // TODO: refactor
        cModule* srcHost = parentModule();
        if ( !srcHost )
            error("parent module not found");

        InterfaceTable *itable = check_and_cast<InterfaceTable*>( srcHost->submodule( "interfaceTable" ) );
        if ( !itable )
            error("parent module has no interface table \"interfaceTable\"");

        InterfaceEntry* iface = itable->interfaceByName( "wlan" );
        if ( !iface )
            error("parent module interface table did not contain interface on which packet arrived");

        re->interfaceName = iface->name();
        re->interfacePtr = iface;

        m_networkRoutingTable->addRoutingEntry( re );
    }

    // handle the actual ODSR packet
    ODSRPacket *packet = dynamic_cast<ODSRPacket*>( message );
    if ( packet ) {
        ev << "Got ODSR packet type: " << packet->getOdsrType() << endl;
        if ( packet->getDestination() == m_myAddress )
            handleOdsrPacketForMe( packet );
        else
            handleOdsrPacketForRelay( packet );
    }
    else {
        error( "Incoming message is not a ODSR packet" );
    }
}

void OdsrLayer::handleOdsrPacketForMe( ODSRPacket *packet )
{
    switch ( packet->getOdsrType() ) {
        case ODSR_REQUEST:
            sendReply( packet );
            break;
        case ODSR_REPLY:
            handleDiscoveredRoute( packet );
            break;
        case ODSR_DATA:
            handleIncomingDataPacket( packet );
            break;
        default:
            error( "Doesn't know how to handle this packet!" );
            return;
    }
}

void OdsrLayer::handleOdsrPacketForRelay( ODSRPacket *packet )
{
    // check if we made this packet
    if ( packet->getSource() == m_myAddress ) {
        ev << "I was the one who issued this packet, dropping the packet" << endl;
        delete packet;
        return;
    }

    if ( packet->getOdsrType() == ODSR_REQUEST ) {
        // check if we've already seen this request
        RoutingVector vector = packet->getVector();
        for( uint i = 0; i < vector.size(); ++i ) {
            if ( vector[ i ].address == m_myAddress ) {
                ev << "I've already seen this request, dropping the packet" << endl;
                delete packet;
                return;
            }
        }

        // otherwise, broadcast the message further on the network
        ev << "Broadcasting the request further" << endl;
        RoutingNode myself;
        myself.address = m_myAddress;
        myself.metric = 0;
        vector.push_back( myself );
        packet->setVector( vector );
        sendToNetwork( packet, LL_MANET_ROUTERS );
    }
    else if ( packet->getOdsrType() == ODSR_REPLY || packet->getOdsrType()== ODSR_DATA ) {
        const uint pointer = packet->getPointer();
        ev << "Pointer: " << pointer << endl;
        // the route goes on
        if ( pointer < packet->getVector().size() ) {
            const IPAddress nextHop = packet->getVector()[ pointer ].address;
            ev << "The next hop: " << nextHop << endl;
            packet->setPointer( pointer + 1 );
            sendToNetwork( packet, nextHop );
        }
        // we reached destination, the next hop is our target
        else if ( pointer == packet->getVector().size() ) {
            ev << "ALMOST REACHED DESTINATION" << endl;
            const IPAddress nextHop = packet->getDestination();
            ev << "The next hop: " << nextHop << endl;
            packet->setPointer( pointer + 1 );
            sendToNetwork( packet, nextHop );
        }
        // missed our target node
        else {
            error( "MISSED TARGET NODE" );
        }
    }
    else {
        error( "Doesn't know how to handle this packet!" );
    }
}

void OdsrLayer::handleDiscoveredRoute( ODSRPacket *packet )
{
    const IPAddress &destination = packet->getSource();
    ev << "Route discovery for: " << destination << endl;
    ev << "The route goes through: " << packet->getVector() << endl;

    // check if we are still interested in this reply
    if ( m_pendingReplies.count( destination ) > 0 &&
         m_pendingReplies[ destination ].seqnum == packet->getSeqnum() ) {
        m_pendingReplies[ destination ].vectors.push_back( packet->getVector() );
    }
    else {
        ev << "I'm not interested in this reply, dropping the packet" << endl;
    }

    delete packet;
}

void OdsrLayer::handleIncomingDataPacket( ODSRPacket *packet )
{
    ev << "Decapsulating data packet to transfer up to generator" << endl;

    // get the IP control info block
    IPControlInfo *control = check_and_cast<IPControlInfo*>( packet->controlInfo() );
    if ( !control )
        error( "Why does the packet I got from IP have no control info attached?" );

    // replace the source address
    control->setSrcAddr( packet->getSource() );

    cMessage *message = packet->decapsulate();
    message->setControlInfo( control );
    send( message, "toGenerator" );
}

void OdsrLayer::processQueueTimeout( QueueTimeout *timeout )
{
    const IPAddress destination = timeout->getDestination();
    ev << "Queue timeout for: " << destination << endl;
    delete timeout;

    // sanity check
    if ( m_pendingReplies.count( destination ) == 0 ) {
        error( "I have no pending replies set for this destination!" );
    }

    // choose one of the routes collected so far for this destination
    vector<RoutingVector> vectors = m_pendingReplies[ destination ].vectors;
    m_pendingReplies.erase( destination );
    if ( vectors.empty() ) {
        ev << "No routes to this destination have been discovered, dropping all packets" << endl;
        ev << "Dropped: " << m_odsrPacketQueue->drop( destination ) << endl;
        return;
    }

    const Hops hops = m_odsrRoutingController->decide( vectors );
    ev << "Hops to route through: " << hops << endl;

    // add a new routing entry and dequeue packets
    m_odsrRoutingTable.addHopsFor( destination, hops );
    dequeuePendingPackets( destination );
}

void OdsrLayer::dequeuePendingPackets( const IPAddress &destination )
{
    ev << "Dequeueing pending packets to: " << destination << endl;

    vector<cMessage*> messages = m_odsrPacketQueue->pull( destination );
    uint num = messages.size();
    ev << "Number of packets to send: " << num << endl;
    for ( uint i = 0; i < num; ++i ) {
        // allocate a new ODSR packet
        ODSRPacket *data = new ODSRPacket();
        data->setOdsrType( ODSR_DATA );
        data->setSource( m_myAddress );
        data->setDestination( destination );
        data->setSeqnum( 0 );

        // determine the route to destination
        const Hops hops = m_odsrRoutingTable.hopsFor( destination );
        const RoutingVector vector = toRoutingVector( hops );
        ev << "Hops: " << hops << endl;
        data->setVector( vector );

        // determine the next hop
        IPAddress nextHop;
        if ( hops.empty() )
            nextHop = destination;
        else
            nextHop = data->getVector()[ 0 ].address;

        ev << "Next hop: " << nextHop << endl;
        data->setPointer( 1 );

        // encapsulate the original message
        data->encapsulate( messages[ i ] );
        sendToNetwork( data, nextHop );
    }

    // erase the routing entry, next call setup will trigger
    // a new routing request
    m_odsrRoutingTable.removeHopsFor( destination );
}

void OdsrLayer::sendToNetwork( ODSRPacket *packet, const IPAddress &nextHop )
{
    if ( packet->controlInfo() ) {
        ev << "Removing existing control info!" << endl;
        packet->removeControlInfo();
    }

    IPControlInfo *control = new IPControlInfo();
    control->setProtocol( ODSR_PROTOCOL );
    control->setDestAddr( nextHop );
    packet->setControlInfo( control );

    // this is meant to emulate processing delay
    // and avoid collisions when broadcasting requests
    // (kind of "backoff")
    const simtime_t ST = 20E-6;
    const double delay = ((double) intrand( 1024 )) * ST;
    ev << "PROCESSING DELAY IS:" << delay << endl;
    sendDelayed( packet, delay, "toNetwork" );
}

void OdsrLayer::sendReply( ODSRPacket *packet )
{
    ev << "Sending reply to: " << packet->getSource() << endl;

    ODSRPacket *reply = new ODSRPacket();
    reply->setOdsrType( ODSR_REPLY );
    reply->setSource( m_myAddress );
    reply->setDestination( packet->getSource() );
    reply->setSeqnum( packet->getSeqnum() );

    // put the list of the route nodes in reverse order
    const RoutingVector vector = packet->getVector();
    RoutingVector reverse;
    for ( uint i = vector.size(); i != 0; --i ) {
        reverse.push_back( vector[ i - 1 ] );
    }
    reply->setVector( reverse );

    ev << "Reply goes via: " << reverse << endl;

    IPAddress nextHop;
    if ( !reply->getVector().empty() )
        nextHop = reply->getVector()[ 0 ].address;
    else
        nextHop = check_and_cast<IPControlInfo*>( packet->controlInfo() )->srcAddr();

    ev << "Next hop: " << nextHop << endl;
    reply->setPointer( 1 );

    delete packet;
    sendToNetwork( reply, nextHop );
}
