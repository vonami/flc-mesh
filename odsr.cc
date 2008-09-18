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

#include "odsr.h"

#include <IPAddress.h>
#include <omnetpp.h>

using namespace std;

// note: walks over the vector in reverse order
Hops extractHops( const RoutingVector &vector )
{
    Hops hops;
    for ( uint i = vector.size(); i != 0; --i ) {
        hops.push_back( vector[ i - 1 ].address );
    }

    return hops;
}

RoutingVector toRoutingVector( const Hops &hops )
{
    RoutingVector vector;
    for( uint i = 0; i < hops.size(); ++i ) {
        RoutingNode node;
        node.address = hops.at( i );
        node.metric = 0;
        vector.push_back( node );
    }

    return vector;
}

ostream& operator<<( ostream &os, const Hops &hops )
{
    os << "{" << endl;
    for ( Hops::const_iterator i = hops.begin(); i != hops.end(); ++i ) {
        os << "  " << *i << std::endl;
    }
    os << "}";
    return os;
}

ostream& operator<<( ostream &os, const RoutingNode &node )
{
    os << "[ " << node.address << " : " << node.metric << " ]";
    return os;
}

ostream& operator<<( ostream &os, const RoutingVector &vector )
{
    os << "{" << endl;
    for ( RoutingVector::const_iterator i = vector.begin(); i != vector.end(); ++i ) {
        os << "  " << *i << std::endl;
    }
    os << "}";
    return os;
}
