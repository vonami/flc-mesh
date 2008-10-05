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

#include "odsrburstygenerator.h"

#include <IPControlInfo.h>
#include <IPAddressResolver.h>

Define_Module( OdsrBurstyGenerator );

int OdsrBurstyGenerator::numInitStages() const
{
    return 4;
}

void OdsrBurstyGenerator::initialize( int stage )
{
    if ( stage == 0 ) {
        m_enabled = par( "enabled" );
        m_burstLength = par( "burstLength" );
        m_burstCounter = m_burstLength;
        m_messageLength = par( "messageLength" );
        m_burstRate = par( "burstRate" );
        m_messageRate = par( "messageRate" );
        m_hostsNumber = par( "hostsNumber" );
        m_sent = 0;
        m_received = 0;
    }

    if ( stage == 3 ) {
        cModule *host = parentModule();
        if ( !host )
            error( "No parent module found" );
        else
            m_selfIndex = host->index();

        // schedule first packet of first burst
        if ( m_enabled ) {
            m_currentDestination = generateAddress();
            scheduleAt( simTime() + intuniform( 0, 10 ) * 2, new cMessage );
        }
    }
}

void OdsrBurstyGenerator::handleMessage( cMessage *message )
{
    if ( !message->isSelfMessage() ) {
        // process incoming packet
        m_received++;
        delete message;
        return;
    }

    if ( !m_enabled )
        return;

    // generate & send packet
    sendToOdsr( generateMessage(), m_currentDestination );

    // if this was the last packet of the burst
    if ( --m_burstCounter == 0 ) {
        // schedule next burst
        m_burstCounter = m_burstLength;
        m_currentDestination = generateAddress();
        scheduleAt( simTime() + exponential( m_burstRate ), message );
    }
    else {
        // schedule next sending within burst
        scheduleAt( simTime() + exponential( m_messageRate ), message );
    }
}

IPAddress OdsrBurstyGenerator::generateAddress() const
{
    ev << "My index: " << m_selfIndex << endl;
    uint index = 0;
    do {
        index = intuniform( 0, m_hostsNumber - 1 );
    } while ( index == m_selfIndex );

    char hostName[32];
    sprintf( hostName, "host[%d]", index );
    ev << "Generated index: " << index << endl;
    ev << "Generated host name: " << hostName << endl;
    const IPAddress dest = IPAddressResolver().resolve( hostName ).get4();
    ev << "Resolved address: " << dest << endl;

    return dest;
}

cMessage* OdsrBurstyGenerator::generateMessage() const
{
    char msgName[32];
    sprintf( msgName, "OdsrBurstyGenerator-%d", m_sent );

    cMessage *payload = new cMessage( msgName );
    payload->setByteLength( m_messageLength );
    return payload;
}

void OdsrBurstyGenerator::sendToOdsr( cMessage *message, const IPAddress &destination )
{
    // send message to the ODSR layer, with the appropriate control info attached
    IPControlInfo *info = new IPControlInfo();
    info->setDestAddr( destination );
    message->setControlInfo( info );

    m_sent++;
    send( message, "toOdsr" );
}

void OdsrBurstyGenerator::finish()
{
    recordScalar( "Sent", m_sent );
    recordScalar( "Received", m_received );
}
