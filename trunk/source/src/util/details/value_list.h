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


/** 
 * The details namespace is used to hide implementation 
 * details, The user should not care about its contents.
 */
namespace details 
{
	template <class ListT, size_t N>
	struct value_list;

	template <class ListT>
	struct value_list<ListT, 0>
	{
		typedef ListT list_type;
		typedef typename boost::mpl::at_c<ListT, 0>::type value_type;
		value_type value;
	};

	template <class ListT, size_t N>
	struct value_list : public value_list<ListT, N-1>
	{
		typedef ListT list_type;
		typedef typename boost::mpl::at_c<ListT, N>::type value_type;
		value_type value;
		value_list() {};
	};

	template <class ListT, template <class> class ParamT, size_t N>
	struct param_value_list;

	template <class ListT, template <class> class ParamT>
	struct param_value_list<ListT,ParamT, 0>
	{
		typedef ParamT<typename boost::mpl::at_c<ListT, 0>::type> value_type;
		value_type value;
	};

	template <class ListT,template <class> class ParamT,  size_t N>
	struct param_value_list : public param_value_list<ListT, ParamT, N-1>
	{
		typedef ParamT<typename boost::mpl::at_c<ListT, N>::type> value_type;
		value_type value;
	};
}
