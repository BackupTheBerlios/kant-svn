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
#ifndef UTILITY_TEMPLATES_H
#define UTILITY_TEMPLATES_H

// This code will be replaced by combined mpl-tuples-[fusion] facilities
// that will be added in the next boost release

#include "details/value_list.h"

/**
 * @brief value_list instantiates every type of the type list ListT.
 * ListT has to be either boost::mpl::vector or boost::mpl::list, or any other type
 * that can be used with boost::mpl::size and boost::mpl::at_c. To access the objects 
 * use field<Index, ValueList> (returns a constant reference) and 
 * field_nc<Index, ValueList>( returna a non-constant reference). 
 */
template <class ListT>
struct value_list 
	: public details::value_list<ListT,boost::mpl::size<ListT>::type::value-1> 
{
	typedef ListT list_type;
	value_list() {};
};

/**
 * constant accessor of elements in a value_list. 
 * N is the index of the element that should be accessed. 
 * usage: 
 *   field<Index>( list );
 * @param list a constant reference to a value_list<ListT> ( ListT == mpl::list/mpl::vector)
 * @return const-reference to an element in list
 */
template <size_t N, class ValListT>
const typename boost::mpl::at_c< typename ValListT::list_type, N>::type &
field( const ValListT & list )
{
	return static_cast<const details::value_list<typename ValListT::list_type, N> *>(&list)->value;
}

/**
 * non-constant accessor of elements in value_list.
 * N is the index of the element that should be accessed. 
 * usage: 
 *   field_nc<Index>( list );
 * @param list a reference to a value_list<ListT> ( ListT == mpl::list/mpl::vector)
 * @return reference to an element in list
 */
template <size_t N, class ValListT>
typename boost::mpl::at_c< typename ValListT::list_type, N>::type &
field_nc( ValListT & list )
{
	return static_cast<details::value_list<typename ValListT::list_type, N> *>(&list)->value;
}

/**
 * @brief param_value_list instantiates ParamT<type> for every type of 
 * the type list ListT. 
 * ListT has to be either boost::mpl::vector or boost::mpl::list, or any other type
 * that can be used with boost::mpl::size and boost::mpl::at_c. To access the objects 
 * use pfield<Index, ValueList> (returns a constant reference ) 
 */
template <class ListT,template <class> class ParamT>
struct param_value_list : public details::param_value_list<ListT, ParamT, boost::mpl::size<ListT>::type::value - 1>
{
	typedef ListT list_type;
};


/**
 * Acts like the function field but, operates on a param_value_list. 
 * returns a reference to the Nth element of the param_value_list. 
 */
template <size_t N, class ParamValListT, template <class> class ParamT>
typename details::param_value_list<typename ParamValListT::list_type,ParamT, N>::value_type &
pfield( ParamValListT& list )
{
	return static_cast<details::param_value_list<typename ParamValListT::list_type, ParamT, N> *>(&list)->value;
}

/**
 * Acts like the function field but, operates on a param_value_list. 
 * returns a const-reference to the Nth element of the param_value_list. 
 */
template <size_t N, class ParamValListT, template <class> class ParamT>
const typename details::param_value_list<typename ParamValListT::list_type,ParamT, N>::value_type &
pfield( const ParamValListT& list )
{
	return static_cast<details::param_value_list<typename ParamValListT::list_type, ParamT, N> *>(&list)->value;
}

/**
 * Access a param_value_list by requesting a certain type
 */
template <class Type, class ListT, template <class> class ParamT> 
ParamT<Type> & 
unique_pfield( param_value_list<ListT,ParamT>& list )
{
	return static_cast<details::param_value_list<ListT, ParamT,  boost::mpl::find<ListT, Type>::type::pos::value> *>(&list)->value;
}


/**
 * Access a param_value_list by requesting a certain type
 */
template <class Type, class ListT, template <class> class ParamT> 
const ParamT<Type> & 
unique_pfield( const param_value_list<ListT,ParamT>& list )
{
	return static_cast<const details::param_value_list<ListT, ParamT, boost::mpl::find<ListT, Type>::type::pos::value> *>(&list)->value;
}

#endif
