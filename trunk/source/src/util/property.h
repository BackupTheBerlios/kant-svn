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

#ifndef XETA_PROPERTY_H
#define XETA_PROPERTY_H
#include <boost/shared_ptr.hpp>
#include <boost/mpl/void_fwd.hpp>
#include <cctype>

using namespace boost;

template <typename T, typename Compound = mpl::void_> 
struct setter
{
	virtual void acquire( Compound* obj = 0 ) {};
	virtual T& get_ref( Compound* obj = 0 ) = 0;
	virtual void release(  Compound* obj = 0 ) {};	
	virtual ~setter() {}
};

template <typename T, typename Compound = mpl::void_>
struct getter
{
	virtual void acquire( const Compound* obj = 0 ) { };
	virtual T const& get_ref( const Compound* obj = 0 ) = 0;
	virtual void release( const Compound* obj = 0 ) {};	
	virtual ~getter() {}
};

template <typename T, typename Compound = mpl::void_> 
struct container
{
	struct read_access 
	{
		virtual void reset() = 0;
		virtual bool test_position() = 0;
		virtual void next() = 0;
		virtual const T& get_current() = 0;
		virtual ~read_access() {};
	};
	
	struct write_access
	{
		virtual void reset() = 0;
		virtual bool test_position() = 0;
		virtual void next() = 0;
		virtual T& get_current() = 0;

		virtual void append( T const& element ) = 0;
		virtual ~write_access() {};
	};
	
	virtual void rel_ptr( Compound * compound_object, T* object ) = 0;
	
	typedef boost::shared_ptr<read_access> read_access_ptr;
	typedef boost::shared_ptr<write_access> write_access_ptr;
	virtual read_access_ptr create_read_access( const Compound* obj = 0 ) const = 0;
	virtual write_access_ptr create_write_access( Compound* obj = 0 ) const = 0;
	virtual ~container() {}
};

struct member_offset
{
	ptrdiff_t diff;

	explicit member_offset( ptrdiff_t d ) : diff(d) {}

	template <typename Element, typename Compound>
	const Element& get_const_ref( const Compound *ptr )
	{
		return *reinterpret_cast<const Element*>(reinterpret_cast<const char*>(ptr) + diff);
	}

	template <typename Element, typename Compound>
	inline Element& get_ref( Compound *ptr )
	{
		return *reinterpret_cast<Element*>(reinterpret_cast<char*>(ptr) + diff);
	}

};

#include "setter.h"
#include "getter.h"

// fold_backward<seq, boost::tuples::null_type, boost::tuples::cons<_2, _1> >
/*
  getter bedient get aus der value_property!
template <typename T>
struct getter
{

	virtual 
};

template <typename T>
struct setter
{
};
*/

template <typename T, typename Compound = mpl::void_>
struct value_property
{
	typedef T value_type;
	typedef Compound compound_type;
	enum { IsContainer = 0, IsSingleValue};
	boost::shared_ptr< setter<T,Compound> > set;
	boost::shared_ptr< getter<T,Compound> > get;

	template <typename Getter, typename Setter>
	void init(Getter g, Setter s) {
		get = gen_get(g);
		set = gen_set(s);
	}

	void init(ptrdiff_t offset) {
		member_offset m(offset);
		set = boost::shared_ptr<setter<value_type, compound_type> >
			( new setter_offset<value_type, compound_type>( m ) );
		get = boost::shared_ptr<getter<value_type, compound_type> >
			( new getter_offset<value_type, compound_type>( m )	);
	}

	/**
	 * Get Pointer (get_ptr) will either acquire a reference to 
	 * the sub object or create a new object, dependending on the 
	 * property type
	 */
	T* get_ptr( Compound * compound_object ) const
	{
		set->acquire( compound_object );
		return &set->get_ref( compound_object );
	}
	
	/**
	 * rel_ptr(Release pointer) will remove the object, acquired by get_ptr. 
	 * In case of a value_property it will be simply released.
	 * If a container_property is used, the function will gain 
	 * write access and append the object before deletion.
	 * If the acquired object is deleted at all also depends on the 
	 * container type.
	 */
	void rel_ptr( Compound * compound_object, T* object ) const
	{
		set->release( compound_object );
	}
};

template <typename T, typename Compound = mpl::void_>
struct container_property 
{
	typedef T value_type;
	typedef Compound compound_type;
	enum { IsSingleValue = 0, IsContainer};
	boost::shared_ptr< container<T,Compound> > container;

	/**
	 * Get Pointer (get_ptr) will either acquire a reference to 
	 * an object or create a new object, dependending on the 
	 * property type
	 */
	T* get_ptr( Compound * compound_object ) const
	{
		return new T;
	}

	/**
	 * rel_ptr(Release pointer) will remove the object, acquired by get_ptr. 
	 * In case of a value_property it will be simply released.
	 * If a container_property is used, the function will gain 
	 * write access and append the object before deletion.
	 * If the acquired object is deleted at all also depends on the 
	 * container type.
	 */
	void rel_ptr( Compound * compound_object, T* object ) const
	{
		container->rel_ptr( compound_object, object );
	}
};

#endif

