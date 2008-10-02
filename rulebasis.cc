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

#include "rulebasis.h"
#include <algorithm>

using namespace std;

RuleBasis::RuleBasis()
{
}

void RuleBasis::addRule( const Rule &rule )
{
    m_rules.push_back( rule );
}

float RuleBasis::crispOutput( float hopsNumber, float metric ) const
{
    float numerator = 0;
    float denominator = 0;

    for ( int i = 0; i < m_rules.size(); ++i ) {
        float mf_hops = m_rules[ i ].mfHops.membership( hopsNumber );
        float mf_metric = m_rules[ i ].mfMetric.membership( metric );
        if ( mf_hops == 0  || mf_metric == 0 )
            continue;

        float height = min( mf_hops, mf_metric );
        float area = m_rules[ i ].mfOutput.area( height );
        numerator += m_rules[ i ].mfOutput.peak() * area;
        denominator += area;
    }

    return numerator / denominator;
}
