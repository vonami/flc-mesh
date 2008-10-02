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

#ifndef MEMBERSHIPFUNCTION_H
#define MEMBERSHIPFUNCTION_H

class MembershipFunction
{
public:

    MembershipFunction();
    MembershipFunction( float left, float peak, float right );

    float peak() const;
    float membership( float value ) const;
    float area( float height ) const;

private:

    float m_left;
    float m_peak;
    float m_right;
};

#endif // MEMBERSHIPFUNCTION_H
