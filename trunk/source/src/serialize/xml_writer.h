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

#ifndef XETA_XML_WRITER_H
#define XETA_XML_WRITER_H
#include "xml_path.h"
namespace xml
{

namespace writer_details {
template<typename PathElement, typename PropertyMapType>
struct wrap_properties;

template<typename PropertyMapType>
struct wrap_properties<boost::mpl::void_, PropertyMapType>
{
	wrap_properties( PropertyMapType const& map )
	{}
	void next() const { }
	bool test_position() const { return true; }
	bool should_repeat() const { return false; }
	void init( void const* obj) {}
};

template<>
struct wrap_properties<boost::mpl::void_, boost::mpl::void_>
{
	wrap_properties( boost::mpl::void_ const& map ) {} 
	void next() const { }
	bool test_position() const { return true; }
	bool should_repeat() const { return false; }
	void init( void const* obj) {}
};


template<typename LeftT, typename RightT, typename PropertyMapType>
struct wrap_properties<details::or_combiner<LeftT,RightT>, PropertyMapType> : 
	public wrap_properties<LeftT, PropertyMapType> , 
	public wrap_properties<RightT, PropertyMapType>
{
	wrap_properties( PropertyMapType const& map )
		: wrap_properties<LeftT, PropertyMapType>( map ),
		wrap_properties<RightT, PropertyMapType>( map )
	{
	}

	bool should_repeat() const 
	{ 
		return wrap_properties<LeftT, PropertyMapType>::should_repeat() 
			||  wrap_properties<RightT, PropertyMapType>::should_repeat(); 
	}
	void next() const {  wrap_properties<LeftT, PropertyMapType>::next(); 
			wrap_properties<RightT, PropertyMapType>::next();  }
	bool test_position() const { return wrap_properties<LeftT, PropertyMapType>::test_position() 
			&&	wrap_properties<RightT, PropertyMapType>::test_position();}
	void init( void const* obj) {
		wrap_properties<LeftT, PropertyMapType>::init( obj );
		wrap_properties<RightT, PropertyMapType>::init( obj );
	}
};

template<typename KeyType, typename ValueType, typename PropertyMapType>
struct wrap_properties<details::character_index<KeyType,ValueType>, PropertyMapType> : 
	public wrap_properties<typename find_type<PropertyMapType,KeyType>::value_type, ValueType>
{
	typedef  wrap_properties<typename find_type<PropertyMapType,KeyType>::value_type, ValueType> base_type;

	wrap_properties( PropertyMapType const & map ) : base_type( map. template get<KeyType>()) {}
											   
	bool should_repeat() const { return base_type::should_repeat(); }
	void next() const { base_type::next();  }
	bool test_position() const { return base_type::test_position(); }
	void init( void const* obj) { base_type::init( obj ); }
};


template<typename KeyType, typename ValueType,  typename PropertyMapType>
struct wrap_properties<details::attribute<KeyType, ValueType>, PropertyMapType> : 
	public wrap_properties<typename find_type<PropertyMapType,KeyType>::value_type, ValueType> 
{
	typedef  wrap_properties<typename find_type<PropertyMapType,KeyType>::value_type, ValueType> base_type;
	
	wrap_properties( PropertyMapType const& map ) : base_type( map. template get<KeyType>()) {}
		
	bool should_repeat() const { return base_type::should_repeat(); }
	void next() const { base_type::next();  }
	bool test_position() const { return base_type::test_position(); }

	void init( void const* obj) { base_type::init( obj ); }
};

template<typename IdType, typename T, typename CompoundType, typename TargetType>
struct wrap_properties< ::details::holder<elem<IdType,T>, CompoundType>, TargetType> 
{
	typedef TargetType target_type; 
	typedef value_property<T,CompoundType> property_type;
	property_type const& prop;
	
	wrap_properties( property_type const& p ) : prop( p ) {}

	bool should_repeat() const { return false; }
	void next() const {   }
	bool test_position() const { return true; }

	void init( void const* obj) { }
	T const& get_current( void const* obj ) { prop.get->acquire( static_cast<CompoundType const*>( obj ) ); return prop.get->get_ref( static_cast<CompoundType const*>( obj ) ); }
	void release_current( void const* obj ) { prop.get->release( static_cast<CompoundType const*>( obj ) ); }
};

template<typename IdType, typename T, typename CompoundType, typename TargetType>
struct wrap_properties< ::details::holder<con<IdType,T>, CompoundType>, TargetType> 
{
	typedef TargetType target_type; 
	typedef container_property<T,CompoundType> property_type;
	property_type const& prop;
	typename container<T, CompoundType>::read_access_ptr it_abstraction;

	wrap_properties( property_type const& p ) : prop( p ) {}

	bool should_repeat() const { return true; }

	void next() const { it_abstraction->next(); }
	bool test_position() const { return it_abstraction->test_position(); }
	void init( void const* obj) { it_abstraction = prop.container->create_read_access( static_cast<CompoundType const*>( obj ) ); }
	T const& get_current( void const* obj ) { return it_abstraction->get_current(); }
	void release_current( void const* obj ) {  }
};



}

/**
 * The wrap_properties template tracks the current access to ...
 * Propertymap/C++Object to the path.
 */
template <typename Path, typename PropertyMapType>
struct wrap_properties : public 
			 writer_details::wrap_properties<
				 xml::details::or_combiner<
				 	typename Path::characters_type, 
					typename Path::attrib_type>,
				 PropertyMapType
				 >
{
	typedef writer_details::wrap_properties<
		details::or_combiner<
			typename Path::characters_type,
			typename Path::attrib_type
				>,
			PropertyMapType
				> base_type;

	wrap_properties( PropertyMapType const& map )
		: base_type( map )
	{		}

	bool should_repeat() const  {  return base_type::should_repeat(); }
	void next() const {  base_type::next(); }
	bool test_position() const { return base_type::test_position(); }
	void init( void const* obj) { base_type::init( obj ); }

};

}

#endif

