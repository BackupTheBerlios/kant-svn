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

#include <vector>
#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/mpl/void.hpp>
#include <boost/mpl/vector.hpp>
#define DEBUG
#define XETA_PRINT_FUNCTION
#include "debug.h"
#include "xml_parser.h"
#include "xml_serialize.h"
using namespace xml;
struct Data{};
struct Name{};

struct Node
{
	private:
		std::vector<Node*> nodes;
		std::string name,data;
	public:
		void print( std::ostream & out ) const;
				typedef property_map<
					boost::mpl::vector<	 con<Node>, elem<Name, std::string>	,elem<Data,std::string> >,
					Node 
				> type_i;
		
		static type_i const& get_info();
};
void Node::print( std::ostream& out ) const
{
	out << "Node " << this << std::endl << nodes.size() << std::endl << name << ", " << data << " has (" << std::endl;
	for( std::vector<Node*>::const_iterator it = nodes.begin(), e = nodes.end();
			it != e; ++it )
		(*it)->print( out );
	out << ")" << std::endl;
}

struct RootNode
{
	private:
		std::string tree_name;
		std::vector<Node*> nodes;
	public:
		void print( std::ostream & out ) const;
		typedef property_map< boost::mpl::vector< elem<Name,std::string>, con<Node> >, RootNode > type_i;
		
		static type_i const& get_info();
};

#define MEMBER_OFFSET(Type, member) member_offset(reinterpret_cast<size_t>(&reinterpret_cast<Type*>(1)->member) - reinterpret_cast<size_t>(reinterpret_cast<Type*>(1)))

void RootNode::print( std::ostream& out ) const
{
	out << "RootNode " << nodes.size() << " hui " << std::endl << tree_name << " has (" << std::endl;
	for( std::vector<Node*>::const_iterator it = nodes.begin(), e = nodes.end();
			it != e; ++it )
		(*it)->print( out );
	out << ")" << std::endl;
}

RootNode::type_i const& RootNode::get_info()
{
	static bool called = false;
	static type_i  map;

	if( !called )
	{
		called = true;
		map.get<Name>().get = gen_get<std::string,RootNode>( MEMBER_OFFSET(RootNode,tree_name) );
		map.get<Name>().set = gen_set<std::string,RootNode>( MEMBER_OFFSET(RootNode,tree_name) ); 

		map.get<Node>().container = boost::shared_ptr< container<Node,RootNode> >
			(new stl_container<Node,RootNode,std::vector<Node*> >( 
				gen_get<std::vector<Node*>,RootNode>(MEMBER_OFFSET(RootNode, nodes ) ),
				gen_set<std::vector<Node*>,RootNode>(MEMBER_OFFSET(RootNode, nodes ) )
					));
	}
	return map;
}

Node::type_i const& Node::get_info()
{
	static bool called = false;
	static type_i  map;
	if( !called )
	{
		called = true;
		map.get<Name>().get = gen_get<std::string,Node>( MEMBER_OFFSET(Node,name) );
		map.get<Name>().set = gen_set<std::string,Node>( MEMBER_OFFSET(Node,name) ); 

		map.get<Data>().get = gen_get<std::string,Node>( MEMBER_OFFSET(Node,data) );
		map.get<Data>().set = gen_set<std::string,Node>( MEMBER_OFFSET(Node,data) ); 


		map.get<Node>().container = boost::shared_ptr< container<Node,Node> >
			(new stl_container<Node,Node,std::vector<Node*> >( 
				gen_get<std::vector<Node*>,Node>(MEMBER_OFFSET(Node, nodes ) ),
				gen_set<std::vector<Node*>,Node>(MEMBER_OFFSET(Node, nodes ) )
					));
	}
	return map;
}
	

void parse_xml( RootNode& obj, std::string const& filename)
{
	boost::shared_ptr<Receiver> basic_node;
	basic_node = xml::gen_object_node(
			xml::sub_tag<Node>( Node::get_info(), "node")
			.attributes( 
				xml::attribute.assign<Name>("name")
				| xml::attribute.assign<Data>("data")
				)
			.sub_tags(
				xml::link_tag<Node>( basic_node, "node" )
				),
			Node::get_info()
			);
	boost::shared_ptr<Receiver> root_node = xml::gen_root_node( 
			xml::root_tag( RootNode::get_info(), "root_node")
			.attributes( xml::attribute.assign<Name>("name") )
			.sub_tags(
				xml::link_tag<Node>( basic_node, "node" )
				)
			);

	Parser p;
	try{
		p.parse( root_node, filename.c_str(), &obj );
		std::cout << "PRINTING:" << std::endl << std::endl;
		obj.print( std::cout );	
		root_node->print( &obj, std::cout );

	}catch ( std::exception &e){
		std::cout << "std::exception " << e.what()<< std::endl;
	}catch (...){
		std::cout << "Exception"<< std::endl;
	}


}

int main()
{
	RootNode Root;
	parse_xml(  Root, "recursive.xml" );
}


