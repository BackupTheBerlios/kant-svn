/***************************************************************************
 *   Copyright (C) 2004 by Andreas Pokorny                                 *
 *                                                                         *
 * This file is part of the Serializer.                                    *
 *                                                                         *
 * Serializer is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU Lesser General Public License as          *
 * published by the Free Software Foundation; either version 2 of the      *
 * License, or (at your option) any later version.                         *
 *                                                                         *
 * Serializer is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this software; if not, write to the Free Software            *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               *
 ***************************************************************************/

#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>

#define DEBUG
#define XETA_PRINT_FUNCTION
#include "debug.h"
#include "example.h"

quote::quote()
{
	DEBUG_COUT("Create:: " << this << " enz:" << &enzness << " text:" << &text << std::endl );
}

int& quote::get_enzness()
{
	DEBUG_COUT(" this = " << this  << std::endl);
	return enzness;
}
int const& quote::get_enzness() const
{
	DEBUG_COUT(" this = " << this << std::endl);
	return enzness;
}

std::string const& quote::get_text() const 
{
	DEBUG_COUT(" this = " << this << std::endl);
	return text;
}
void quote::set_text( std::string const& t ) 
{
	DEBUG_COUT(" this = " << this<< ", t = " << t << std::endl);
	text = t;
}


void quote::print(  std::ostream &o  ) const
{
	o << " Enzness:" << enzness << std::endl; 
	o << " Text:" << text << std::endl; 
}

int& kunType::get_iq()
{
	DEBUG_COUT(" this = " << this << std::endl);
	return iq;
}

int const& kunType::get_iq() const
{
	DEBUG_COUT(" this = " << this << std::endl);
	return iq;
}
float& kunType::get_leetq() 
{
	DEBUG_COUT(" this = " << this << std::endl);
	return leetq;
}

float const& kunType::get_leetq() const
{
	DEBUG_COUT(" this = " << this << std::endl);
	return leetq;
}

std::vector<quote>& kunType::get_examples()
{
	DEBUG_COUT(" this = " << this << std::endl);
	return leet_examples;
}
std::vector<int>& kunType::get_values()
{
	DEBUG_COUT(" this = " << this << std::endl);
	return values;
}

std::vector<quote> const& kunType::get_examples()const
{
	DEBUG_COUT(" this = " << this << std::endl);
	return leet_examples;
}

std::vector<int> const& kunType::get_values()const
{
	DEBUG_COUT(" this = " << this << std::endl);
	return values;
}



void kunType::print(  std::ostream &o  ) const
{
	o << "IQ:" << iq << std::endl;
	o << "LeetQ:" << leetq << std::endl;
	o << "LeetExamples:" << std::endl;
	for( int i = 0, e = leet_examples.size(); i < e;  ++i ) 
		o << leet_examples[i];
	o << "Values:";
	std::ostream_iterator<int> out( o, " ");
	std::copy( values.begin(), values.end(), out );
}
std::ostream & operator <<( std::ostream & out, const kunType & kun)
{
	kun.print( out );
	return out;
}
std::ostream & operator <<( std::ostream & out, const quote& q)
{
	q.print( out );
	return out;
}

