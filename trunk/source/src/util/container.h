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

#ifndef ZYNOT_CONTAINER_IMPL_H
#define ZYNOT_CONTAINER_IMPL_H
#define DEBUG
#include "../serialize/debug.h" 
namespace details {
/*template<typename T, typename ValueType, typename ParamT>
struct const_iterator_access;

template<typename T, typename ParamT>
struct const_iterator_access<T,T,ParamT>
{
	static T const& access( ParamT & param ) { return *param; }
};

template<typename T, typename ParamT>
struct const_iterator_access<T,T*,ParamT>
{
	static T const& access( ParamT & param ) { return **param; }
};

template<typename T, typename ValueType, typename ParamT>
struct iterator_access;

template<typename T, typename ParamT>
struct iterator_access<T,T,ParamT>
{
	static T & access( ParamT & param ) { return *param; }
	static T const& transfer( T const& param ) { return param; }
};

template<typename T, typename ParamT>
struct iterator_access<T,T*,ParamT>
{
	static T & access( ParamT & param ) { return **param; }
	static T *const transfer( T & param ) { return &param; }
};
*/
/**
 * split pointer and non-pointer containers here
 */
template< typename T, typename ValueType, typename Compound, typename ContainerType>
struct stl_container_implementation;
// TODO SmartPointer Container.. 

/**
 * Value container specialisation:
 *
 */
template< typename T, typename Compound, typename ContainerType>
struct stl_container_implementation<T,T,Compound, ContainerType>
	: public container<T, Compound>
{
	typedef typename ContainerType::iterator iterator;
	typedef typename ContainerType::const_iterator const_iterator;

	typedef boost::shared_ptr< getter<ContainerType, Compound> > container_get;
	typedef boost::shared_ptr< setter<ContainerType, Compound> > container_set;
	container_get get_container;
	container_set set_container;

	stl_container_implementation( container_get a_getter,  container_set a_setter ) 
		: get_container( a_getter ),  set_container( a_setter ) {}
	struct stl_read : public container<T,Compound>::read_access
	{
		container_get get_container;
		Compound const* obj;
		const_iterator begin,end;

		void reset()
		{
			begin = get_container->get_ref( obj ).begin(); 
			end = get_container->get_ref( obj ).end();
		}
		bool test_position() { return begin != end; }
		void next() { ++begin; }
		const T& get_current() { return *begin; } 

		stl_read( Compound const* o, container_get get ) 
			: get_container(get), obj(o) 
		{ 
			get_container->acquire( obj );
			reset(); 
		}
		~stl_read() { get_container->release( obj ); }
	};

	struct stl_write : public container<T,Compound>::write_access
	{
		container_set set_container;
		Compound * obj;
		iterator begin,end;

		void reset()
		{
			begin = set_container->get_ref( obj ).begin();
			end = set_container->get_ref( obj ).end(); 
		}

		bool test_position() { return begin != end;}
		void next() { ++begin;}

		T& get_current() { return *begin; } 

		void append( T const& element ) 
		{
			std::cout << "#################### Appending an object to a VALUE vector with " << set_container->get_ref( obj ).size() << " objects " << std::endl;
			set_container->get_ref( obj ).push_back( element);
		} 

		stl_write( Compound* o, container_set set ) 
			: set_container(set), obj(o)
		{
			set_container->acquire(obj); 
			reset(); 
		}

		~stl_write() { set_container->release(obj); }
	};

	
	/**
	 * Releases the Object used to fill data into the container.
	 * This code assumes that the container makes a deep copy of that
	 * object. 
	 */
	void rel_ptr( Compound * compound_object, T* object )
	{
		stl_write writer( compound_object, set_container );
		writer.append( *object );
		delete object;
	}

};

/**
 * Pointer container.
 * 
 */
template< typename T, typename Compound, typename ContainerType>
struct stl_container_implementation<T,T*,Compound, ContainerType> 
	: public container<T, Compound>
{
	typedef typename ContainerType::iterator iterator;
	typedef typename ContainerType::const_iterator const_iterator;

	typedef boost::shared_ptr< getter<ContainerType, Compound> > container_get;
	typedef boost::shared_ptr< setter<ContainerType, Compound> > container_set;

	container_get get_container;
	container_set set_container;

	stl_container_implementation( container_get a_getter,  container_set a_setter ) 
		: get_container( a_getter ),  set_container( a_setter ) {}

	
	struct stl_read : public container<T,Compound>::read_access
	{
		container_get get_container;
		Compound const* obj;
		const_iterator begin,end;

		void reset()
		{
			begin = get_container->get_ref( obj ).begin(); 
			end = get_container->get_ref( obj ).end();
		}
		bool test_position() { return begin != end;}
		void next() { ++begin;  }
		const T& get_current() { return **begin; } 

		stl_read( Compound const* o, container_get get ) 
			: get_container(get), obj(o) 
		{ 
			get_container->acquire( obj );
			reset(); 
		}
		~stl_read()
		{
			get_container->release( obj );
		}
	};

	struct stl_write : public container<T,Compound>::write_access
	{
		container_set set_container;
		Compound * obj;
		iterator begin,end;

		void reset()
		{
			begin = set_container->get_ref( obj ).begin();
			end = set_container->get_ref( obj ).end(); 
		}

		bool test_position() { return begin != end;}
		void next() { ++begin;}

		T& get_current() { return **begin; } 

		void append( T const& element ) 
		{
			std::cout << "#################### Appending an object to a POINTER vector with " << set_container->get_ref( obj ).size() << " objects " << std::endl;
			// ugly but i have to do that:
			// I get                    T const& E
			// amd the container wants  T * const
			set_container->get_ref( obj ).push_back( const_cast<T*>(&element) );
		} 

		stl_write( Compound* o, container_set set ) 
			: set_container(set), obj(o)
		{
			set_container->acquire(obj); 
			reset(); 
		}

		~stl_write()
		{
			set_container->release(obj); 
		}
	};

	/**
	 * Releases the Object used to fill data into the container.
	 * This code assumes that the container takes ownership of the 
	 * pointer, or that the final user of the container is aware 
	 * of the objects ownership in the pointer container.
	 */
	void rel_ptr( Compound * compound_object, T* object )
	{
		stl_write writer( compound_object, set_container );
		writer.append( *object );
		// We do not delete the pointer here
	}

};


}

template <typename T, typename Compound, typename ContainerType>
struct stl_container : public details::stl_container_implementation<T, typename ContainerType::value_type,Compound , ContainerType>
{
	typedef details::stl_container_implementation<T,typename ContainerType::value_type,Compound , ContainerType> base_type;
	typedef typename base_type::stl_read stl_read;
	typedef typename base_type::stl_write stl_write;
	typedef typename ContainerType::iterator iterator;
	typedef typename ContainerType::const_iterator const_iterator;
	typedef boost::shared_ptr< getter<ContainerType, Compound> > container_get;
	typedef boost::shared_ptr< setter<ContainerType, Compound> > container_set;
	typedef typename container<T,Compound>::read_access_ptr read_access_ptr;
	typedef typename container<T,Compound>::write_access_ptr write_access_ptr;
	
	stl_container( container_get a_getter,  container_set a_setter ) 
		: base_type( a_getter ,  a_setter ) {}

	read_access_ptr create_read_access( const Compound* obj ) const
	{ return read_access_ptr(new stl_read( obj, this->get_container ) ); };
	
	write_access_ptr create_write_access( Compound* obj ) const
	{ return write_access_ptr(new stl_write( obj, this->set_container ) ); };

};

#endif

