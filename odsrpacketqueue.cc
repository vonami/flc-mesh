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

#include "odsrpacketqueue.h"

#include <IPControlInfo.h>

using namespace std;

OdsrPacketQueue::OdsrPacketQueue( int length )
    : m_length( length )
{
    m_queue.setName( "OdsrPacketQueue" );
    m_queue.clear();
}

OdsrPacketQueue::~OdsrPacketQueue()
{
}

vector<cMessage*> OdsrPacketQueue::pull( const IPAddress &address )
{
    ev << "Pulling for address: " << address << endl;

    vector<cMessage*> packets;
    for( cQueue::Iterator it( m_queue ); !it.end(); it++ ) {
        // they always have control info, verified by the ODSR layer
        // when it enqueues packets
        cMessage *packet = check_and_cast<cMessage*>( it() );
        IPControlInfo* controlInfo = check_and_cast<IPControlInfo*>( packet->controlInfo() );
        if ( controlInfo->destAddr() == address ) {
            packets.push_back( packet );
        }
    }

    // remove these packets from the queue
    for ( int i = 0; i < packets.size(); ++i ) {
        m_queue.remove( packets[ i ] );
    }

    return packets;
}

uint OdsrPacketQueue::drop( const IPAddress &address )
{
    ev << "Dropping for address: " << address << endl;

    uint num = 0;
    vector<cMessage*> packets; 
    for( cQueue::Iterator it( m_queue ); !it.end(); it++ ) {
        // they always have control info, verified by the ODSR layer
        // when it enqueues packets
        cMessage *packet = check_and_cast<cMessage*>( it() );
        IPControlInfo* controlInfo = check_and_cast<IPControlInfo*>( packet->controlInfo() );
        if ( controlInfo->destAddr() == address ) {
            num++;
            packets.push_back( packet );
        }
    }

    // remove these packets from the queue
    for( int i = 0; i < packets.size(); ++i ) {
        delete m_queue.remove( packets[ i ] );
    }

    return num;
}

void OdsrPacketQueue::push( cMessage *message )
{
    if ( m_queue.length() == m_length ) {
        cMessage *packet = check_and_cast<cMessage*>( m_queue.pop() );
        IPControlInfo* controlInfo = check_and_cast<IPControlInfo*>( packet->controlInfo() );
        ev << "Dropping a packet from the queue: " << controlInfo->destAddr() << endl;
        delete packet;
    }

    m_queue.insert( message );
    ev << "Enqueued the packet, count: " << m_queue.length() << endl;
}

int OdsrPacketQueue::length() const
{
    return m_queue.length();
}
