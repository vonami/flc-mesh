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

#ifndef ODSRROUTINGTABLE_H
#define ODSRROUTINGTABLE_H

#include "odsr.h"

#include <IPAddress.h>
#include <omnetpp.h>
#include <vector>
#include <map>

struct OdsrRoutingEntry
{
    Hops hops;
    bool neighbour;
};

class OdsrRoutingTable
{
public:

    OdsrRoutingTable();
    ~OdsrRoutingTable();

    void addHopsFor( const IPAddress &destination, const Hops &hops );
    Hops hopsFor( const IPAddress &destination ) const;
    bool hasHopsFor( const IPAddress &destination ) const;
    void removeHopsFor( const IPAddress &destination );

private:

    std::map<IPAddress, OdsrRoutingEntry> m_entries;
};

#endif // ODSRROUTINGTABLE_H
