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

#ifndef PROPERTY_MAP_H
#define PROPERTY_MAP_H
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/find_if.hpp>

template <typename A, typename B = A>
struct elem
{
	typedef A id_type;
	typedef B element_type;
};
template <typename A, typename B = A>
struct con
{
	typedef A id_type;
	typedef B container_element_type;
};

namespace details 
{
	template <typename element, typename Compound>
	struct holder;

	template <typename A, typename B, typename Compound>
	struct holder< con<A,B>, Compound >
	{
		typedef A id_type;
		typedef container_property< B, Compound> property_type;
		property_type prop;
	};

	template< typename element, typename Compound>
	struct holder
	{
		typedef typename element::id_type id_type;
		typedef value_property<typename element::element_type, Compound> property_type;
		property_type prop;
	};

	template <typename Current, typename End, typename Compound>
	struct map_element;

	template <typename Current, typename Compound>
	struct map_element<Current, Current, Compound>
	{
	};

	template <typename Current, typename End, typename Compound>
	struct map_element : public map_element<typename boost::mpl::next<Current>::type, End, Compound>, 
						 public holder<typename Current::type,Compound>
	{
	};


/*	template< typename IdType, typename ElementType>
	struct test_element;

	template< typename IdType, typename ElementType>
	struct test_element<IdType, elem< IdType, ElementType> >
	{
		enum { result = 1};
	};

	template< typename IdType, typename ElementType>
	struct test_element<IdType, con< IdType, ElementType > >
	{
		enum { result = 1};
	};

	template< typename IdType, typename ElementType>
	struct test_element
	{
		enum { result = 0};
	};

	template< int Matches, typename IdType, typename Current, typename End>
	struct get_result;

	template< typename IdType, typename Current, typename End>
	struct get_result<1, IdType, Current, End>
	{
		typedef typename Current::type type;
	};

	template< typename IdType, typename Current, typename End>
	struct get_result<0, IdType, Current, End>
	{
		typedef typename Current::next next;
		typedef typename get_result<
			test_element<IdType, typename next::type>::result, 
			IdType, 
			next,
			End
				>::type type;
	};

	template<typename SearchedType>
	struct element_does_not_exist_in_that_map;

	template< typename IdType, typename Current>
	struct get_result<0, IdType, Current, Current>
	{
		typedef element_does_not_exist_in_that_map<IdType> type;
	};

	
	template< typename begin, typename end, typename IdType>
	struct finder
	{
		typedef typename get_result< 
			test_element<IdType,typename begin::type>::result,
			IdType,
			begin,
			end>::type type;
	};*/

	template<typename SearchedType>
	struct element_does_not_exist_in_that_map;

	template<typename BrokenTypeFound>
	struct element_should_not_be_in_that_map;

	template <typename Element, typename IdType>
	struct find_predicate;

	template <typename IdTypeInElement, typename ElementType, typename IdType>
	struct find_predicate< elem<IdTypeInElement,ElementType>, IdType>
	{ enum { value = 0}; };

	template <typename IdTypeInElement, typename ElementType, typename IdType>
	struct find_predicate< con<IdTypeInElement,ElementType>, IdType>
	{ enum { value = 0}; };

	template <typename IdTypeInElement, typename ElementType>
	struct find_predicate< con<IdTypeInElement,ElementType>, IdTypeInElement>
	{ enum { value = true}; };

	template <typename IdTypeInElement, typename ElementType>
	struct find_predicate< elem<IdTypeInElement,ElementType>, IdTypeInElement>
	{ enum { value = true}; };

	template <typename T, typename IdType>
	struct find_predicate
	{
		element_should_not_be_in_that_map<T> type;
	};

	template<typename Iterator, typename EndIterator, typename TypeSearched>
	struct process_find_if_result;

	template<typename EndIterator, typename TypeSearched>
	struct process_find_if_result<EndIterator,EndIterator,TypeSearched>
	{
		typedef element_does_not_exist_in_that_map<TypeSearched> type;
	};

	template<typename Iterator, typename EndIterator, typename TypeSearched>
	struct process_find_if_result
	{
		typedef typename Iterator::type type;
	};

}
/*
template<typename Property_map, typename IdType>
struct find_type
{
	typedef details::finder<
		typename Property_map::begin_type, 
		typename Property_map::end_type, 
		IdType> finder_type;
	
	typedef typename finder_type::type found_type;
	typedef details::holder< found_type, typename Property_map::compound_type> value_type;
};
*/

template<typename Property_map, typename IdType>
struct find_type
{
	// find_predicate throws a compiler error on a bad element
	typedef typename boost::mpl::find_if<
		typename Property_map::mpl_vector,
		details::find_predicate<boost::mpl::_1,IdType>
			>::type 
			last_iterator;
	
	// process_find_if_result throws a compiler error, if type is not found
	typedef typename details::process_find_if_result<
		last_iterator,
		typename Property_map::end_type,
		IdType
			>::type found_type;
	typedef details::holder< found_type, typename Property_map::compound_type> value_type;
};
template <typename MplVector, typename Compound>
class property_map
{
	public:
		typedef MplVector mpl_vector;
		typedef Compound compound_type;
		typedef property_map<mpl_vector,compound_type> self_type;
		typedef typename boost::mpl::begin<mpl_vector>::type begin_type;
		typedef typename boost::mpl::end<mpl_vector>::type end_type;
	private:
		details::map_element< begin_type, end_type, Compound> map;
	public:
		template<typename IdType>
		typename find_type<self_type,IdType>::value_type::property_type const& 
		get() const
		{
			return static_cast< typename find_type<self_type,IdType>::value_type const&>(map).prop;
		}

		template<typename IdType>
		typename find_type<self_type,IdType>::value_type::property_type & 
		get()
		{
			return static_cast< typename find_type<self_type,IdType>::value_type &>(map).prop;
		}

        template<typename IdType, typename Getter, typename Setter>
        void init(Getter g, Setter s) {
            get<IdType>().init(g, s);
        }

        template<typename IdType>
        void init(ptrdiff_t offset) {
            get<IdType>().init(offset);
        }
};

#endif 

