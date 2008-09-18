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

#ifndef ODSR_H
#define ODSR_H

#include <IPAddress.h>
#include <iostream>
#include <vector>

const short ODSR_PROTOCOL = 2525;
const IPAddress LL_MANET_ROUTERS = "224.0.0.90";
const uint ODSR_REQUEST = 0;
const uint ODSR_REPLY = 1;
const uint ODSR_DATA = 2;

typedef std::vector<IPAddress> Hops;

struct RoutingNode
{
    IPAddress address;
    unsigned int metric;
};

typedef std::vector<RoutingNode> RoutingVector;

Hops extractHops( const RoutingVector &vector );
RoutingVector toRoutingVector( const Hops &hops );

std::ostream& operator<<( std::ostream &os, const Hops &hops );
std::ostream& operator<<( std::ostream &os, const RoutingNode &node );
std::ostream& operator<<( std::ostream &os, const RoutingVector &vector );

#endif // ODSR_H
