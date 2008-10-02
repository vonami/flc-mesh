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

#include "membershipfunction.h"

MembershipFunction::MembershipFunction()
    : m_left( 0 ), m_peak( 0 ), m_right( 0 )
{
}

MembershipFunction::MembershipFunction( float left, float peak, float right )
    : m_left( left ), m_peak( peak ), m_right( right )
{
}

float MembershipFunction::peak() const
{
    return m_peak;
}

float MembershipFunction::membership( float value ) const
{
    if ( value < m_left || value > m_right )
        return 0;

    if ( value <= m_peak && value >= m_left )
        return ( value - m_left ) / ( m_peak - m_left );

    if ( value > m_peak && value <= m_right )
        return 1 - ( value - m_peak ) / ( m_right - m_peak );
}

float MembershipFunction::area( float height ) const
{
    return ( m_right - m_left ) * height * ( 1 - 0.5 * height );
}
