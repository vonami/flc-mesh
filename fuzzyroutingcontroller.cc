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

#include "fuzzyroutingcontroller.h"
#include <iostream>

using namespace std;

FuzzyRoutingController::FuzzyRoutingController()
{
    // initialize the rule basis
}

Hops FuzzyRoutingController::decide( const std::vector<RoutingVector> &vectors ) const
{
    float minRating = 0;
    uint minIndex = -1;

    // for every route calculate its crisp rating
    for ( int i = 0; i < vectors.size(); ++i ) {
        uint hops = vectors[ i ].size();
        uint metric = 0;
        for ( int j = 0; j < hops; ++j )
            metric += vectors[ i ][ j ].metric;

        float rating = m_basis.crispOutput( hops, metric );
        if ( rating < minRating ) {
            minRating = rating;
            minIndex = i;
        }
    }

    if ( minIndex == -1 ) {
        cerr << "The current input is not decidable" << endl;
        minIndex = 0;
    }

    return extractHops( vectors.at( minIndex ) );
}
