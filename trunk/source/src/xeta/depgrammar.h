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

#ifndef XETA_DEPENDENCY_GRAMMAR_DEFINTION_H
#define XETA_DEPENDENCY_GRAMMAR_DEFINTION_H
using namespace boost::spirit;
/**
 * Comparator defines a symbol table that accepts 
 * all possible syntax of version operators.
 */
struct Comparator  : public symbols<unsigned int>
{
	Comparator()
	{
		add
			( "eq-", 1)
			( "=", 1)
			( "ge-", 2)
			( ">=", 2)
			( "gt-", 3)
			( ">", 3)
			( "le-", 4)
			( "<=", 4)
			( "lt-", 5)
			( "<", 5)
			;
	}
};


/**
 * BoolOps is a symbol table defining boolean
 * operators.
 */
struct BoolOps  : public symbols<unsigned int>
{
	BoolOps()
	{
		add
			( "and", 1)
			( "&&", 1)
			( "&", 1)
			( "or", 2)
			( "||", 2)
			( "|", 2)
			;
	}
};

/**
 * Dependency is the grammar of our dependency 
 * language. You can define if-else clauses, test
 * flags, installed packages with their versions,
 * require or refuse a certain versions of packages,
 * and define flag requirements for installed packages.
 */
struct Dependency : public grammar<Dependency>
{
	/**
	 * These enums are used within the ast_tree 
	 * iteration.
	 */
	enum 
	{
		List,
		IfClause,
		ElsePart,
		BoolExpression,
		BoolTerm,
		PackSpec,
		Flag,
		Version,
		InternalName,
		LeafBoolExpr
	};

	/**
	 * The definition structure is templated by the type of 
	 * scanner used. 
	 * The grammar expects a scanner without skip parser.
	 */
	template <typename ScannerT>
	struct definition
	{
		rule<ScannerT, parser_context<>, parser_tag<List> > list;
		rule<ScannerT, parser_context<>, parser_tag<IfClause> > if_clause;
		rule<ScannerT, parser_context<>, parser_tag<ElsePart> > else_part;
		rule<ScannerT, parser_context<>, parser_tag<BoolExpression> > bool_expression;
		rule<ScannerT, parser_context<>, parser_tag<BoolTerm> > bool_term;
		rule<ScannerT, parser_context<>, parser_tag<PackSpec> > pack_spec;
		rule<ScannerT, parser_context<>, parser_tag<Flag> > flag;
		rule<ScannerT, parser_context<>, parser_tag<Version> > version;
		rule<ScannerT, parser_context<>, parser_tag<InternalName> > internal_name;
		rule<ScannerT, parser_context<>, parser_tag<LeafBoolExpr> > leaf_bool_expr;
		chset<char> S;
		Comparator version_comparator;
		BoolOps bool_combiner;


		
		definition( Dependency const& /*self*/)
			: S (" \r\t\n")
		{
			//  Start grammar definition
			leaf_bool_expr 
				= 
				root_node_d[str_p("flag:")] >> flag 
				| root_node_d[str_p("pkg:")] >> pack_spec
				;

			internal_name 
				= 
				token_node_d[ +chset<>("0-9a-zA-Z_") ]
				;

			version 
				=
				infix_node_d[  token_node_d[+digit_p ]  % '.'] >>
				!(token_node_d['_' >> ( str_p("rc") | 'a' | 'b' )] >> token_node_d[ +digit_p ] ) >>
				!(token_node_d[str_p("-r")] >> token_node_d[+digit_p] )
				;

			flag 
				= 
				!( ch_p('+') | '-' ) >> 
				token_node_d[ +chset<>("0-9a-zA-Z_")]
				;

			pack_spec
				=
				!ch_p('!') >> 
				(
				 version_comparator >> 
         internal_name >> 
         !(discard_node_d[ch_p('[') >> *S] >> flag % discard_node_d[( *S >> ',' >> *S )] >> discard_node_d[*S >> ']']) 
				 >> discard_node_d[*S >> '-' >> *S] >> version
				 | 
         internal_name 
         >> !(discard_node_d[ch_p('[') >> *S] >> flag % discard_node_d[( *S >> ',' >> *S )] >> discard_node_d[*S >> ']']) 
				)
				;

			bool_expression
				=   
				bool_term
				>> *( root_node_d[bool_combiner] >> bool_term )
				;

			bool_term
				=  
				discard_node_d[*S] >>  
				(
				 leaf_bool_expr 
				 |   inner_node_d['(' >> bool_expression >> ')']
				 |   (root_node_d[ch_p('!')] >> bool_term)
				)
				>> discard_node_d[*S]
				;

			if_clause 
				= 
				root_node_d[str_p("if")] >> discard_node_d[*S >> '('] >> bool_expression  >> discard_node_d[')' >> *S] >> 
				inner_node_d[ '{' >> list >> '}']  >>
				!( else_part ) 
				//>> discard_node_d[*S]
				;

			else_part
				=
				discard_node_d[*S] >> root_node_d[str_p("else")] >> discard_node_d[*S] >> inner_node_d[ '{' >> list >> '}']
				//>> discard_node_d[*S]
				;

			list = 
				discard_node_d[ *S ] >>
				*(
						( 
						 if_clause
						 |  pack_spec
						)
						>> discard_node_d[*S]
				 )
				;

			//  End grammar definition

			// turn on the debugging info.
#ifdef BOOST_SPIRIT_DEBUG
			BOOST_SPIRIT_DEBUG_RULE(list);
			BOOST_SPIRIT_DEBUG_RULE(if_clause);
			BOOST_SPIRIT_DEBUG_RULE(else_part);
			BOOST_SPIRIT_DEBUG_RULE(internal_name);
			BOOST_SPIRIT_DEBUG_RULE(pack_spec);
			BOOST_SPIRIT_DEBUG_RULE(version);
			BOOST_SPIRIT_DEBUG_RULE(flag);
			BOOST_SPIRIT_DEBUG_RULE(bool_expression);
			BOOST_SPIRIT_DEBUG_RULE(bool_term);
			BOOST_SPIRIT_DEBUG_RULE(leaf_bool_expr);
#endif
		}

		rule<ScannerT, parser_context<>, parser_tag<List> > const&
			start() const { return list; }
	};
};

#endif

