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
#include <boost/shared_ptr.hpp>
#include <boost/mpl/void.hpp>
#include <boost/mpl/vector.hpp>


#define DEBUG
#include "debug.h"
#include "xml_path.h"
#include "xml_serialize.h"

using namespace std;
using namespace xml;
#include "example.h"

struct Quote_EnzNess{};
struct Quote_Text{};
struct KunType_Iq{};
struct KunType_LeetQ{};
struct KunType_LeetExamples{};
struct KunType_Values{};
typedef property_map<
	boost::mpl::vector<
		elem<Quote_EnzNess,int>,
		elem<Quote_Text,std::string>
	>,
	quote
	> quote_map;

typedef property_map<
	boost::mpl::vector<
		elem<KunType_Iq,int>,
		elem<KunType_LeetQ,float>,
		con<KunType_LeetExamples,quote>,
		con<KunType_Values,int>
	>,
	kunType
	> kuntype_map;


quote_map q_m;
kuntype_map k_m;

void init_km() 
{
	DEBUG_COUT("init_km()" << std::endl);
	k_m.get<KunType_Iq>().init<>( static_cast<int const&(kunType::*)() const>(&kunType::get_iq), static_cast<int&(kunType::*)()>(&kunType::get_iq) );

	k_m.get<KunType_LeetQ>().init<>( static_cast<float const&(kunType::*)() const>(&kunType::get_leetq), static_cast<float&(kunType::*)()>(&kunType::get_leetq) );

	k_m.get<KunType_Values>().container = boost::shared_ptr<container<int,kunType> >(new stl_container<int,kunType, std::vector<int> >(gen_get(&kunType::get_values), gen_set(static_cast<std::vector<int>& (kunType::*)()>(&kunType::get_values) ) ) );
	k_m.get<KunType_LeetExamples>().container = boost::shared_ptr<container<quote,kunType> >(new stl_container<quote,kunType, std::vector<quote> >(gen_get(&kunType::get_examples), gen_set(static_cast<std::vector<quote>& (kunType::*)()>(&kunType::get_examples) ) ) );

}

void init_qm() 
{
	DEBUG_COUT("init_qm()" << std::endl);
	q_m.get<Quote_EnzNess>().init( static_cast<int const&(quote::*)()const>(&quote::get_enzness),
			static_cast<int&(quote::*)()>(&quote::get_enzness) );

	q_m.get<Quote_Text>().init( &quote::get_text, &quote::set_text );
}

void parse_xml( kunType& kun_obj, std::string const& filename)
{

	boost::shared_ptr<Receiver> root_node = xml::gen_root_node( 
			xml::root_tag( k_m, "kun_type")
			.attributes( xml::attribute.assign<KunType_LeetQ>("leetq")
				| xml::attribute.assign<KunType_Iq>("iq")
				)
			.sub_tags(
				xml::sub_tag<KunType_LeetExamples>(q_m,"leet_example")
				.attributes(
					xml::attribute.assign<Quote_EnzNess>("enzness")
					)
				.characters<Quote_Text>()
				|
				xml::no_obj_tag( "values" )
				.attributes(
					xml::attribute.assign<KunType_Values>("int")
					)
				)
			);

	Parser p;
	try{
		p.parse( root_node,filename.c_str(), &kun_obj );
		std::cout << "SUCCESS"<< std::endl;
		std::cout << kun_obj;
		std::cout << "SUCCESS"<< std::endl;
		root_node->print( &kun_obj, std::cout );

	}catch ( std::exception &e){
		std::cout << "std::exception " << e.what()<< std::endl;
	}catch (...){
		std::cout << "Exception"<< std::endl;
	}
	/*	boost::shared_ptr<Receiver> quote_node = gen_object_node( 
			q_m,
						
			);*/
}

int main ()
{
	DEBUG_COUT("TEST " << std::endl);
	init_qm();
	init_km();

	quote  a;
	kunType kun_obj;
	parse_xml( kun_obj, "example.xml" );

	std::cout << "Content of parsed data:" << std::endl << kun_obj << std::endl;
	
	/*base_serializer<kunType> *Parser = xml::generate_serializer( 
			xml::root_tag<kunType>("kun").
				attributes(				
					xml::attribute.assign<0>("iq") | 
					xml::attribute.assign<float,1>("leetq") 
					).
				sub_tags(

					sub_tag<2>("leet_example").
						attributes(  xml::attribute.assign<0>("value")  ).
						characters<1>()  
					|	

					sub_tag<-1>("bla").
						attributes(  xml::attribute.assign<3>("value") )
					
					)
			);*/

	/*Parser->parse_xml("foo.xml", a);

	std::cout << a << std::endl;

	Parser->print_xml(a, "foo2.xml");*/

}

