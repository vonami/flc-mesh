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

#ifndef ODSRLAYER_H
#define ODSRLAYER_H

#include "odsrpacket_m.h"
#include "queuetimeout_m.h"
#include "odsrpacketqueue.h"
#include "odsrroutingtable.h"
#include "odsrroutingcontroller.h"

#include <omnetpp.h>
#include <RoutingTable.h>

struct OdsrPendingReply
{
    uint seqnum;
    std::vector<RoutingVector> vectors;
};

class OdsrLayer : public cSimpleModule
{
public:

    Module_Class_Members( OdsrLayer, cSimpleModule, 0 );
    int numInitStages() const;
    void initialize( int stage );
    void finish();
    void handleMessage( cMessage* msg );

private:

    void handleOutgoingDataMessage( cMessage *message );
    void handleIncomingOdsrMessage( cMessage *message );

    void handleOdsrPacketForMe( ODSRPacket *packet );
    void handleOdsrPacketForRelay( ODSRPacket *packet );
    void handleDiscoveredRoute( ODSRPacket *packet );
    void handleIncomingDataPacket( ODSRPacket *packet );

    void processQueueTimeout( QueueTimeout *timeout );
    void dequeuePendingPackets( const IPAddress &destination );
    void sendToNetwork( ODSRPacket *packet, const IPAddress &nextHop );
    void sendReply( ODSRPacket *packet );

private:

    IPAddress m_myAddress;
    uint m_seqnum;
    double m_queueTimeout;

    const char *m_odsrInterfaces;
    IPAddress m_autoassignAddressBase;
    RoutingTable *m_networkRoutingTable;
    OdsrRoutingController *m_odsrRoutingController;

    OdsrPacketQueue *m_odsrPacketQueue;
    OdsrRoutingTable m_odsrRoutingTable;
    std::map<IPAddress, OdsrPendingReply > m_pendingReplies;

    // statistics
    cOutVector m_statQueueSize;
    uint m_statIcmpErrors;
};

#endif // ODSRLAYER_H
