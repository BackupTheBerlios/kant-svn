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


#ifndef EXMAPLE_H
#define EXMAPLE_H
class quote
{
	private:
		int enzness;
		std::string text;
	public:
		int& get_enzness();
		int const& get_enzness() const;

		std::string const& get_text() const;
		void set_text( std::string const& t );

		quote();
		void print( std::ostream &o ) const;
};


class kunType
{
	private:
		int iq;
		float leetq;
		std::vector<quote> leet_examples;
		std::vector<int> values;
	public:
		int& get_iq();
		int const& get_iq() const;

		float& get_leetq();
		float const& get_leetq() const;

		std::vector<quote>& get_examples();
		std::vector<int>& get_values();
		
		std::vector<quote> const& get_examples()const;
		std::vector<int> const& get_values()const;

		void print( std::ostream &o ) const;
};

std::ostream & operator <<( std::ostream & out, const kunType & kun);
std::ostream & operator <<( std::ostream & out, const quote& q);
#endif 

