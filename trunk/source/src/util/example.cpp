/***************************************************************************
 *   Copyright (C) 2004 by Andreas Pokorny <apokorny@zynot.org>            *
 *                                                                         *
 * This file is part of the Xeta package manager.                          *
 *                                                                         *
 * Xeta is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU Lesser General Public License as          *
 * published by the Free Software Foundation; either version 2 of the      *
 * License, or (at your option) any later version.                         *
 *                                                                         *
 * Xeta is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this software; if not, write to the Free Software            *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               *
 ***************************************************************************/

//Usage example of the properties: 
#include "property.h"
#include "setter.h"
#include "getter.h"
#include "container.h"
#include <vector>
#include <iostream>
#include <string>
#include <boost/mpl/vector.hpp>
#include <boost/tuple/tuple.hpp>
#include "property_map.h"
using namespace boost;
struct ex
{
	int i; 
	std::string o;
	std::vector<int> bla;
};

struct IntID{};
struct NameStr{};
struct Content{};
typedef property_map< 
	boost::mpl::vector<
		elem<IntID,int>, 
		elem<NameStr,std::string>, 
		con<Content,int >
		>, 
	ex> ex_map;

typedef property_map< 
	boost::mpl::vector<
		elem<IntID,int>, 
		elem<std::string>, 
		con<int >
		>, 
	ex> other_ex_map;

#define MEMBER_OFFSET(Type, member) (reinterpret_cast<size_t>(&reinterpret_cast<Type*>(1)->member) - reinterpret_cast<size_t>(reinterpret_cast<Type*>(1)))
int main()
{
	//tuple_type a;
	ex_map a;
	a.get<IntID>( ).set = gen_set<int,ex>(member_offset( MEMBER_OFFSET(ex, i) ) );
	a.get<IntID>( ).get = gen_get<int,ex>(member_offset( MEMBER_OFFSET(ex, i) ) );
	a.get<NameStr>().set = gen_set<std::string,ex>(member_offset( MEMBER_OFFSET(ex, o) ) );
	a.get<NameStr>().get = gen_get<std::string,ex>(member_offset( MEMBER_OFFSET(ex, o) ) );
	a.get<Content>().container = boost::shared_ptr< container<int,ex> >( new stl_container<int,ex, std::vector<int> >( 
			gen_get<std::vector<int>,ex>(member_offset( MEMBER_OFFSET(ex, bla) ) ),
			gen_set<std::vector<int>,ex>(member_offset( MEMBER_OFFSET(ex, bla) ) )
			));

	ex object;

	container<int,ex>::write_access_ptr	writer = a.get<Content>().container->create_write_access( &object );
	writer->append( 12 );
	writer->append( 12 );
	writer->append( 12 );
	writer->append( 14 );
	
	writer->reset();
	writer->get_current() = 7;
	
	container<int,ex>::read_access_ptr reader = a.get<Content>().container->create_read_access( &object );
	for( ; reader->test_position(); reader->next() )
		std::cout << reader->get_current() << std::endl;

	for( ; writer->test_position(); writer->next() )
		writer->get_current() = 9;

	reader->reset();
	for( ; reader->test_position(); reader->next() )
		std::cout << reader->get_current() << std::endl;
}

