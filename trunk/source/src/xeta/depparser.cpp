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
// #define BOOST_SPIRIT_DEBUG
#include <boost/spirit/core.hpp>
#include <boost/spirit/symbols/symbols.hpp>
#include <boost/spirit/utility/chset.hpp>
#include <boost/spirit/utility/loops.hpp>
#include <boost/spirit/tree/parse_tree.hpp>
#include <boost/spirit/tree/ast.hpp>
#ifdef BOOST_SPIRIT_DEBUG
#include <boost/spirit/tree/tree_to_xml.hpp>
#endif
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <iostream>
#include "depgrammar.h"
#include "packagespec.h"
#include "depparser.h"

using namespace std;
using namespace boost::spirit;
using namespace xeta;

namespace {
	typedef char const*                              iterator_t;
	typedef tree_match<iterator_t>                   parse_tree_match_t;
	typedef tree_parse_info<iterator_t> 			 tree_parse_info_t;
	typedef parse_tree_match_t::const_tree_iterator  iter_t;

	typedef ast_match_policy<iterator_t> match_policy_t;
	typedef skip_parser_iteration_policy<chset<char> > iteration_policy_t;
	typedef scanner_policies<iteration_policy, match_policy_t, action_policy> scanner_policy_t;
	typedef scanner<iterator_t, scanner_policy_t> scanner_t;
	typedef rule<scanner_t> rule_t;

	const Comparator version_operators;
	const BoolOps boolean_operators;
		
	inline std::string get_string( const iter_t & it )
	{
		return std::string( it->value.begin(), it->value.end() );
	}
}

struct xeta::DependencyParser::internal
{
	internal();
	
	Dependency gram;
#ifdef BOOST_SPIRIT_DEBUG
	std::map<parser_id, std::string> rule_names;
#endif 
	tree_parse_info_t p_info;

	// Methods to receive the question list:
	void bool_get_questions( const iter_t& it, RespondList& questions );
	void get_questions( const iter_t& it, RespondList& questions );

	// Methods to evaluate the dependency string based on the responds
	void get_packages( const iter_t& it, const std::set<std::string>& flags, std::list<PackageInfo>& packages, RespondIterator& pos );
	// returns true if the expression is valid depending on the current parameters:
	bool eval_bool( const iter_t& it, const std::set<std::string>& flags, RespondIterator& pos);
	// eats in the RespondsList but does not create entries in the package list
	void skip_responds( const iter_t& it, RespondIterator& pos, bool in_bool );
	// reads a packagespec with more than a internal string ( either flags or version )
	void get_package( const iter_t& it, PackageInfo & current );
};

xeta::DependencyParser::internal::internal()
{
#ifdef BOOST_SPIRIT_DEBUG
	rule_names[Dependency::List] ="list";
	rule_names[Dependency::IfClause] ="if_clause";
	rule_names[Dependency::ElsePart] ="else_part";
	rule_names[Dependency::BoolExpression] ="bool_expression";
	rule_names[Dependency::BoolTerm] ="bool_term";
	rule_names[Dependency::PackSpec] ="pack_spec";
	rule_names[Dependency::Flag] ="flag";
	rule_names[Dependency::Version] ="version";
	rule_names[Dependency::InternalName] ="internal_name";
	rule_names[Dependency::LeafBoolExpr] ="leaf_bool_expr";
#endif 
}
bool xeta::DependencyParser::internal::eval_bool( const iter_t& it, std::set<std::string> const& flags, RespondIterator& pos )
{
	bool ret;
	const iter_t sub = it->children.begin();
	switch( it->value.id().to_long() )
	{
		case Dependency::BoolTerm:
			if( get_string( it ) == "!" ) // should be always "!" .. remove when testing is done
				return !eval_bool( sub, flags, pos );
			else return eval_bool( sub, flags, pos );// we should never touch that
		case Dependency::BoolExpression:
			{
				unsigned int *data = boost::spirit::find( boolean_operators, get_string( it ).c_str() );
				if( !data ) throw std::runtime_error("Boolean operator expected"); // rather impossible :)

				switch( *data )
				{
					case 1:
						if( ! eval_bool( sub, flags, pos ) )
						{
							skip_responds( sub, pos, true );
							return false;
						}
						return eval_bool( sub, flags, pos );
					case 2:
						if( eval_bool( sub, flags, pos ) )
						{
							skip_responds( sub, pos, true );
							return true;
						}
						return eval_bool( sub, flags, pos );
				}
			}

		case Dependency::LeafBoolExpr: 
			return eval_bool( sub, flags, pos );
		case Dependency::PackSpec:
			ret = pos->first;
			++pos;
			break;
		case Dependency::Flag:
			return flags.find( get_string( it ) ) != flags.end();

		case Dependency::InternalName:
			ret = pos->first;
			++pos;
			break;
	}
	return ret;
}
void xeta::DependencyParser::internal::skip_responds( const iter_t& it, RespondIterator& pos, bool in_bool )
{
	// We handle the two possible states ( being in a bool expression - eat every package ) and 
	// ( being out side of a if(...) clause - proceed doing nothing )
	// using a boolean value 
	if( !in_bool )
	{
		switch( it->value.id().to_long() )
		{
			case Dependency::BoolTerm:
			case Dependency::BoolExpression:
			case Dependency::LeafBoolExpr:
				in_bool = true; break;
			default: break;
		}
		for( 
				iter_t b = it->children.begin(), e = it->children.end();
				b != e; ++b )
			skip_responds( b, pos, in_bool );
	}
	else {
		switch( it->value.id().to_long() )
		{
			// every RespondList entry must have a corresponding InternalName 
			case Dependency::InternalName:
				++pos; break;
			case Dependency::PackSpec:
				++pos; break;
			default:
				for( 
						iter_t b = it->children.begin(), e = it->children.end();
						b != e; ++b )
					skip_responds( b, pos, in_bool );
				break;

		}
	}

}

void xeta::DependencyParser::internal::get_package( const iter_t& it, PackageInfo & current )
{
	for( 
			iter_t b = it->children.begin(), e = it->children.end();
			b != e; ++b )
	{
		switch( b->value.id().to_long() )
		{
			case Dependency::InternalName:
				current.set_name( get_string( b ) );
				break;
			case Dependency::Flag:
				if( b->children.begin() == b->children.end() )
				{
					current.add_flag( get_string( b ), true );
				}
				else 
				{
					iter_t flag_string =  ( b->children.begin() + 1);

					current.add_flag( get_string( flag_string ), (*b->children.begin()->value.begin() == '+') );
				}
				break;
			case Dependency::Version:
				{
					VersionSpec version = current.get_version();
					for( 
							iter_t v = b->children.begin(), e = b->children.end();
							v != e; ++v )
					{
						try
						{
							version.add_number(boost::lexical_cast<unsigned int>( get_string( v ) ));
						}
						catch(boost::bad_lexical_cast &)
						{ 
							try {
								if( get_string( v ) == "_b" )
									version.set_beta( boost::lexical_cast<unsigned int>( get_string( ++v) ) );
								else if( get_string( v ) == "_a" )
									version.set_alpha( boost::lexical_cast<unsigned int>( get_string( ++v) ) );
								else if( get_string( v ) == "_rc" )
									version.set_release_candidate( boost::lexical_cast<unsigned int>( get_string( ++v) ) );
								else if( get_string( v ) == "-r" )
									version.set_revision( boost::lexical_cast<unsigned int>( get_string( ++v) ) );
							}
							catch(boost::bad_lexical_cast &) {
								throw std::runtime_error("number expected");
							}
						}
					}

					current.set_version( version );
				}
				break;
				// The inverse/block operator '!', and the version operators do not have 
				// their own rule id, so they are handled here:
			default:
				{
					VersionSpec spec = current.get_version();

					if( get_string( b ) == "!" )  
						spec.set_block( true );

					else  {
						unsigned int *data = boost::spirit::find( version_operators, get_string( b ).c_str() );

						if( !data ) 
							std::runtime_error("Version opetator expected");

						switch( *data ) 	{
							case 1: spec.set_operator( VersionSpec::Is );  break;
							case 2: spec.set_operator( VersionSpec::Greater );  break;
							case 3: spec.set_operator( VersionSpec::GreaterThan );  break;
							case 4: spec.set_operator( VersionSpec::Less );  break;
							case 5: spec.set_operator( VersionSpec::LessThan );  break;
						}

					}

					current.set_version( spec );
				}
				break;
		}

	}
}

void xeta::DependencyParser::internal::get_packages( const iter_t& it, const std::set<std::string>& flags, std::list<PackageInfo>& packages, RespondIterator& pos )
{

	switch( it->value.id().to_long() )
	{
		case Dependency::List:
		case Dependency::ElsePart:
			for( 
					iter_t b = it->children.begin(), e = it->children.end();
					b != e; ++b )
				get_packages( b, flags, packages, pos );
			break;
		case Dependency::IfClause:
			if( eval_bool( it->children.begin(), flags, pos ) )
			{
				get_packages( it->children.begin() + 1, flags, packages, pos );
				if( it->children.size() == 3 )
					skip_responds( it->children.begin() + 2, pos, false );
			}
			else
			{
				skip_responds( it->children.begin() + 1, pos, false );
				if(	it->children.size() == 3 )
					get_packages( it->children.begin() + 2, flags, packages, pos );
			}
			break;

			// Do nothing when hiting the other alternatives 
		case Dependency::PackSpec:
			{
				PackageInfo info;
				packages.push_back( info );
				PackageInfo & current = packages.back();

				get_package( it, current );

			}
			break;

		case Dependency::InternalName:
			packages.push_back( PackageInfo(get_string( it ) ) );
			break;
	}
}

void xeta::DependencyParser::internal::bool_get_questions( const iter_t& it, RespondList& questions )
{
	switch( it->value.id().to_long() )
	{
		case Dependency::BoolTerm:
		case Dependency::BoolExpression:
		case Dependency::LeafBoolExpr:
			for( 
					iter_t b = it->children.begin(), e = it->children.end();
					b != e; ++b )
				bool_get_questions( b, questions );
			break;
		case Dependency::PackSpec:
			{
				std::pair<bool, PackageInfo> info;
				questions.push_back( info );
				std::pair<bool, PackageInfo>& current = questions.back();
				current.first = false;

				get_package( it, current.second );
			}
			break;
		case Dependency::Flag:
		case Dependency::Version:
			break;

		case Dependency::InternalName: // just a package name without any flags or version specs
			questions.push_back(std::pair<bool, PackageInfo>( false, PackageInfo(get_string( it )) ));
			break;
	}
}

void xeta::DependencyParser::internal::get_questions( const iter_t& it, RespondList& questions )
{

	switch( it->value.id().to_long() )
	{
		case Dependency::PackSpec:
		case Dependency::Flag:
		case Dependency::Version:
		case Dependency::InternalName:
			break;

		case Dependency::BoolTerm:
		case Dependency::BoolExpression:
		case Dependency::LeafBoolExpr:
			for( 
					iter_t b = it->children.begin(), e = it->children.end();
					b != e; ++b )
				bool_get_questions( b, questions );
			break;


		case Dependency::List:
		case Dependency::IfClause:
		case Dependency::ElsePart:
		default:
			for( 
					iter_t b = it->children.begin(), e = it->children.end();
					b != e; ++b )
				get_questions( b, questions );

			break;


			// Do nothing when hiting the other alternatives 
	}
}


	xeta::DependencyParser::DependencyParser()
: data( new internal )
{
}

xeta::DependencyParser::~DependencyParser()
{
	delete data;
}

	xeta::DependencyParser::DependencyParser( DependencyParser const & cp )
: data( new internal(*cp.data) )
{
}

DependencyParser& xeta::DependencyParser::operator=( DependencyParser const& cp)
{
	if( this != &cp )
	{
		delete data;
		data = new internal(*cp.data);
	}
	return *this;
}


bool xeta::DependencyParser::parse( const std::string & str )
{
	data->p_info = ast_parse( str.c_str(), data->gram);

#ifdef BOOST_SPIRIT_DEBUG
	tree_to_xml(cout, data->p_info.trees, str , data->rule_names);
#endif
	return data->p_info.full;
}

void xeta::DependencyParser::get_questions( RespondList& questions )
{
	if( data->p_info.full )
		for( iter_t it = data->p_info.trees.begin(), e = data->p_info.trees.end();it != e;++it  )
			data->get_questions( it, questions );	
}

void xeta::DependencyParser::get_packages( const std::set<std::string>& flags, std::list<PackageInfo>& packages, const  RespondList& responds )
{
	if( data->p_info.full )
	{
		RespondIterator b = responds.begin();
		for( iter_t it = data->p_info.trees.begin(), e = data->p_info.trees.end();it != e;++it  )
			data->get_packages( it, flags, packages, b );	
	}

}


