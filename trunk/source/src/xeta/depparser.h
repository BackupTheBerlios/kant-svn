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

#ifndef DEPENDENCY_PARSER_H_G
#define DEPENDENCY_PARSER_H_G
#include <set>
#include <string>
#include <list>
namespace xeta
{

/**
 * @brief DependencyParser encapsulates the dependency language parser 
 * generated by boost::spirit. 
 * After parsing the input stream, the user has to get the list of 
 * requested package informations. 
 */
class DependencyParser 
{
  private:
    struct internal;
    internal *data;
  public:
	typedef std::list<std::pair<bool,PackageInfo> > RespondList;
	typedef RespondList::const_iterator RespondIterator;
    DependencyParser();
    DependencyParser( const DependencyParser& cp );
    DependencyParser& operator= ( const DependencyParser& cp);
    ~DependencyParser();
	bool parse( const std::string & dep_string );
	void get_questions( RespondList & questions );
	void get_packages( const std::set<std::string>& flags, std::list<PackageInfo>& packages, const RespondList& responds );
};

}
#endif

