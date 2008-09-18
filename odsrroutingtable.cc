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

#include "odsrroutingtable.h"

using namespace std;

OdsrRoutingTable::OdsrRoutingTable()
{
}

OdsrRoutingTable::~OdsrRoutingTable()
{
}

void OdsrRoutingTable::addHopsFor( const IPAddress &destination, const Hops &hops )
{
    if ( m_entries.count( destination ) > 0 ) {
        ev << "WARNING: Already contains this routing entry!";
        return;
    }

    m_entries[ destination ].hops = hops;
    m_entries[ destination ].neighbour = hops.empty();
}

Hops OdsrRoutingTable::hopsFor( const IPAddress &destination ) const
{
    // avoid inserting empty values
    if ( m_entries.count( destination ) > 0 )
        return m_entries.at( destination ).hops;

    return Hops();
}

bool OdsrRoutingTable::hasHopsFor( const IPAddress &destination ) const
{
    return ( m_entries.count( destination ) > 0 );
}

void OdsrRoutingTable::removeHopsFor( const IPAddress &destination )
{
    if ( m_entries.count( destination ) == 0 ) {
        ev << "WARNING: No such routing entry!";
        return;
    }

    m_entries.erase( destination );
}
