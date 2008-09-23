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

#ifndef ODSRBURSTYGENERATOR_H
#define ODSRBURSTYGENERATOR_H

#include <omnetpp.h>
#include <IPAddress.h>

class OdsrBurstyGenerator : public cSimpleModule
{
public:

    int numInitStages() const;
    void initialize( int stage );
    void handleMessage( cMessage *message );
    void finish();

private:

    IPAddress generateAddress() const;
    cMessage* generateMessage() const;
    void sendToOdsr( cMessage *message, const IPAddress &destination );

private:

    uint m_selfIndex;
    uint m_hostsNumber;
    IPAddress m_currentDestination;
    //uint m_currentIndex;

    bool m_enabled;
    uint m_burstLength;
    uint m_burstCounter;
    uint m_messageLength;
    double m_burstRate;
    double m_messageRate;

    // statistics
    uint m_sent;
    uint m_received;
};

#endif // ODSRBURSTYGENERATOR_H
