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

#ifndef XETA_XML_SERIALIZE
#define XETA_XML_SERIALIZE
#include <vector>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include "../util/property.h"
#include "../util/getter.h"
#include "../util/setter.h"
#include "../util/container.h"
#include "../util/property_map.h"
#include "xml_path.h"
#include "xml_writer.h"

namespace xml
{


/**
 * read_from_string, initializes and object by parsing a string
 * the user should implement specializsations of that function.
 */
template <typename ValueType>
void read_from_string( std::string const& data, ValueType &obj );

template <typename ValueType>
void read_from_string( std::string const& data, ValueType &obj )
{
	std::istringstream input( data );
	input >> obj; 
}

template <>
void read_from_string<std::string>( std::string const& data, std::string &obj )
{
	obj = data;
}

/**
 * write_to_string, writes the object into a string
 */
template <typename ValueType>
void write_to_string( std::string & data, ValueType const& obj );

template <typename ValueType>
void write_to_string( std::string & data, ValueType const& obj )
{
	std::ostringstream output;
	output << obj; 
	data = output.str();
}

template <>
void write_to_string<std::string>( std::string & data, std::string const& obj )
{
	data = obj;
}


/**
 * This structure decides which type should be used when parsing an attribute or 
 * characters data. If especially when the types differ the objects need to be 
 * converted.
 */
template <typename ValueType, typename TargetType> 
struct get_parse_type;

template <typename T> 
struct get_parse_type<T,T>
{
	typedef T type;
};

template <typename T> 
struct get_parse_type<boost::mpl::void_,T>
{
	typedef T type;
};

template <typename ValueType, typename TargetType> 
struct get_parse_type
{
	typedef ValueType type;
};


/**
 * The Character dispatcher either does nothing and returns
 * false when used, or sends the character to the property
 * defined in the map.
 */
template <typename CharacterType, typename PropertyMapType>
struct character_dispatcher;
/**
 * Attribute dispatcher will eat the Attribute type, by walking through the 
 * or_combiner<..>'s and testing the attribute name against the attribute 
 * array. Every dispatcher will then modify the bool array to signalize 
 * which attribute has been parsed. 
 * Behaviour on "false" elements is subject of change, might become a 
 * ObjectReadNode/RootReadNode flag.
 */
template <typename AttributePath, typename PropertyMapType>
struct attribute_dispatcher;

/**
 * The subtag_dispatcher eats the subtags_type of a path element, 
 * by eating the or_combiner<>'s. The dispatch method checks the 
 * name parameter against the value of .get_name(). If the tag has 
 * been found the method returns true to break the loop. 
 * The caller of the dispatcher will then check for false. 
 * false would be interpreted as an error in the xml file .. 
 * subject of change, might become a flag. FIXME
 */
template <typename OtherPathElement, typename PropertyMapType>
struct subtag_dispatcher;


template <typename LeftT, typename RightT , typename PropertyMapType>
struct subtag_dispatcher< details::or_combiner<LeftT, RightT>, PropertyMapType>
{
	typedef subtag_dispatcher< LeftT, PropertyMapType> left_type;
	typedef subtag_dispatcher< RightT, PropertyMapType> right_type;

	static void print( void const* object, details::or_combiner<LeftT, RightT> const& e, PropertyMapType const& map, std::string indent, std::ostream & out )
	{
		left_type::print( object, e.get_left(), map, indent, out );
		right_type::print( object, e.get_right(), map, indent, out );
	}

	static bool dispatch( Parser* p, details::or_combiner<LeftT, RightT> const& e, char const* name, PropertyMapType const& map )
	{
		if( ! subtag_dispatcher< LeftT, PropertyMapType>::dispatch( p, e.get_left(),  name, map ) )
			return subtag_dispatcher< RightT, PropertyMapType>::dispatch( p, e.get_right(),  name, map );
		return true;
	}
};

template <typename PropertyMapType>
struct subtag_dispatcher< boost::mpl::void_, PropertyMapType>
{
	static void print( void const* object, boost::mpl::void_ const& e, PropertyMapType const& map, std::string indent, std::ostream& out  )
	{
	}


	static bool dispatch( Parser* p, boost::mpl::void_ const& e, char const* name, PropertyMapType const& map )
	{
		return false;
	}
};


template <typename Attributes, 
	  typename SubTags, 
	  typename Characters,
	  typename PropertyMapType>
struct subtag_dispatcher< details::tag<boost::mpl::void_, boost::mpl::void_, Attributes, SubTags, Characters>, PropertyMapType>
{
	typedef details::tag<boost::mpl::void_, boost::mpl::void_, Attributes, SubTags, Characters> other_path_type;

	static void print( void const* object, other_path_type const& e, PropertyMapType const& map, std::string indent, std::ostream & out )
	{
		// FIXME Does that work .. or do we need to iterate on wrapper?
		wrap_properties<other_path_type, PropertyMapType> next_wrapper( map );
		next_wrapper.init( object );
		std::string next_indent = indent + "  ";
		for( ;  next_wrapper.test_position() && next_wrapper.should_repeat() ; next_wrapper.next() );
		{
			out << indent << "<" << e.get_name();

			attribute_dispatcher<typename other_path_type::attrib_type, PropertyMapType>::print( object, e.get_attributes(), next_wrapper, out );
			out << ">\n";
			subtag_dispatcher<typename other_path_type::subtags_type, PropertyMapType>::print( object, e.get_subtags(), map, next_indent, out );
			character_dispatcher<typename other_path_type::characters_type, PropertyMapType>::print( object, e.get_characters(), next_wrapper, out );

			out << indent << "</" << e.get_name() << ">\n";
		}
	}
	
	static bool dispatch( Parser* p, other_path_type const& e, char const* name, PropertyMapType const& map)
	{
		if( name == e.get_name() /* || remove namespace == e.get_name() */)
		{
			// Rethink that gen_obejct_node
			p->push_tag( gen_object_node( e, map ), p->get_top().object ); 
			return true;
		}
		else 
			return false;
	}
};

template <typename NewPropertyMap, 
	  typename KeyType,  // in PropertyMapType
	  typename Attributes, 
	  typename SubTags, 
	  typename Characters,
	  typename PropertyMapType>
struct subtag_dispatcher< details::tag<NewPropertyMap, KeyType, Attributes, SubTags, Characters>, PropertyMapType>
{
	typedef details::tag<NewPropertyMap, KeyType, Attributes, SubTags, Characters> other_path_type;
	
	static void print( void const* object, other_path_type const& e, PropertyMapType const& map, std::string indent, std::ostream & out  )
	{
		typedef typename find_type<PropertyMapType, KeyType>::value_type holder_type;
		typedef writer_details::wrap_properties< holder_type, boost::mpl::void_> w_d;
		w_d wrapped_property( map. template get<KeyType>() );
		wrapped_property.init( object );
		
		wrap_properties<other_path_type, typename other_path_type::map_type> next_wrapper( e.get_map() );
		std::string next_indent = indent + "  ";
		for( ; wrapped_property.test_position() && wrapped_property.should_repeat(); wrapped_property.next() )
		{
			void const* next_object = &wrapped_property.get_current( object );
			next_wrapper.init( next_object );
			out << indent << "<" << e.get_name();
			attribute_dispatcher<typename other_path_type::attrib_type, NewPropertyMap>::print( next_object, e.get_attributes(), next_wrapper, out );
			out << ">\n";
			subtag_dispatcher<typename other_path_type::subtags_type, NewPropertyMap>::print( next_object, e.get_subtags(), e.get_map(), next_indent, out );
			character_dispatcher<typename other_path_type::characters_type, NewPropertyMap>::print( next_object, e.get_characters(),  next_wrapper, out );
			out << indent << "</" << e.get_name() << ">\n";
			wrapped_property.release_current( object );
		}
	}

	static bool dispatch( Parser* p, other_path_type const& e, char const* name, PropertyMapType const& map )
	{
		typedef typename find_type<PropertyMapType, KeyType>::value_type::property_type property_type;
		if( name == e.get_name() /* || remove namespace == e.get_name() */)
		{
			void *compound = p->get_top().object;
			p->push_tag( 
					gen_object_node( e, e.get_map() ), 
					map. template get<KeyType>().get_ptr( static_cast<typename property_type::compound_type*>( compound ) )
					); 
			p->get_top().on_removal = boost::bind( 
			//		static_cast<void (*)(property_type const&, typename property_type::compound_type*,typename property_type::value_type *)>
					( &property_type::rel_ptr ),
					map. template get<KeyType>(),
					static_cast<typename property_type::compound_type*>( compound ),
					static_cast<typename property_type::value_type*>( p->get_top().object ) );
			return true;
		}
		else 
			return false;
	}
};

// TODO Add specialisation for predefined ObjectReadNode's in xml paths ( used for recursion )
// ThisType would be an Objectnode then.. 

template <typename KeyType, typename PropertyMapType>
struct subtag_dispatcher< details::predef_tag<KeyType>, PropertyMapType>
{
	typedef details::predef_tag<KeyType> tag_type;

	static void print( void const* object, tag_type const& e, PropertyMapType const& map, std::string indent, std::ostream & out )
	{
		writer_details::wrap_properties<
			typename find_type<PropertyMapType,KeyType>::value_type, 
			typename find_type<PropertyMapType,KeyType>::value_type::property_type::value_type
				>	wrapped_property( map. template get<KeyType>() );
		
		for( wrapped_property.init( object ); wrapped_property.test_position() && wrapped_property.should_repeat(); wrapped_property.next() )
		{
			e.get_object_node( )->print( &wrapped_property.get_current( object ), out, indent );
			wrapped_property.release_current( object );
		}
	}

	static bool dispatch( Parser* p, tag_type const& e, char const* name, PropertyMapType const& map )
	{
		typedef typename find_type<PropertyMapType, KeyType>::value_type::property_type property_type;
		if( name == e.get_name() /* || remove namespace == e.get_name() */ )
		{
			void *compound = p->get_top().object;
			p->push_tag( e.get_object_node(), map. template get<KeyType>().get_ptr( static_cast<typename property_type::compound_type*>( compound ) ) ); 
			p->get_top().on_removal = boost::bind( 
					static_cast<void (property_type::*)(typename property_type::compound_type*, typename property_type::value_type*) const>(&property_type::rel_ptr),
					map. template get<KeyType>(),
					static_cast<typename property_type::compound_type*>( compound ),
					static_cast<typename property_type::value_type*>(p->get_top().object ) );

			return true;
		}
		else 
			return false;
	}
};

/**
 */
template <typename ValueType, typename TargetType, typename PropertyType>
struct parse_element;

template <typename TargetType, typename PropertyType>
struct parse_element<TargetType,TargetType,PropertyType>
{
	static void write( typename PropertyType::value_type const& value, std::ostream & out )
	{
		// parse object
		try {
			std::string data;
			write_to_string<TargetType>( data, value );
			out << data;
		}
		catch(...)
		{
			// throw error message.. 
		}
	}
	
	static void parse( std::string const& data, PropertyType const& prop, void* compound)
	{
		DEBUG_COUT("parse_element<TargetType,TargetType,PropertyType>(" << data << ", prop = " << &prop << ", compound = "  << compound << std::endl);
		// get pointer 
		TargetType* obj = static_cast<TargetType*>(prop.get_ptr( static_cast<typename PropertyType::compound_type*>(compound) ));
	
		// parse object
		try {
			read_from_string<TargetType>( data, *obj );
		}
		catch(...)
		{
			prop.rel_ptr( static_cast<typename PropertyType::compound_type*>(compound),  static_cast<typename PropertyType::value_type*>(obj) );
			// throw error message.. 
		}

		// release pointer
		prop.rel_ptr( static_cast<typename PropertyType::compound_type*>(compound),  static_cast<typename PropertyType::value_type*>(obj) );
	}
};

template <typename ValueType, typename TargetType, typename PropertyType>
struct parse_element
{
	static void write( typename PropertyType::value_type const& value, std::ostream & out )
	{
		try {
			ValueType obj( value ); // type in the xml 
			std::string data;
			write_to_string<ValueType>( data, obj );
			out << data;
		}
		catch(...)
		{
			// throw error message.. 
		}
	}

	static void parse( std::string const& data, PropertyType const& prop, void* compound)
	{
		// create temporary
		DEBUG_COUT("parse_element<ValueType,TargetType,PropertyType>(" << data << ", prop = "<<  &prop << ", compound = "  << compound );
		ValueType obj;
				
		// parse object
		try{
			read_from_string<ValueType>( data, *obj );
		}
		catch(...)
		{
//			throw sth;
		}
		
		// get pointer
		TargetType *target = static_cast<TargetType*>(prop.get_ptr( static_cast<typename PropertyType::compound_type*>(compound) ));
		
		// convert - cast - temporary
		*target = obj;
		
		// release pointer
		prop.rel_ptr( static_cast<typename PropertyType::compound_type*>(compound), static_cast<typename PropertyType::target_type*>(target) );
	}
};



template <typename PropertyMapType>
struct attribute_dispatcher<boost::mpl::void_, PropertyMapType>
{
	template <typename PropertyMapWrapper>
	static void print( void const* object, boost::mpl::void_ const& e, PropertyMapWrapper & wrapper, std::ostream & out )
	{
	}
	static void dispatch( Parser* p, boost::mpl::void_ const& e, PropertyMapType const& map, std::vector<bool> & atts_used, char const ** atts )
	{
		// dont do anything .. the caller has to check whether all attributes have been handled
	}
};

// Possible Variantions: Keytype == mpl::void_ .. obj will be set to true
// ValueType == mpl::void_ the ValueType is picked from the PropertyMapType
// ValueType != PropertyMapType<KeyType> .. the Valuetype will be parsed different then converted .. :eek:
template <typename KeyType, typename ValueType, typename PropertyMapType>
struct attribute_dispatcher<details::attribute<KeyType,ValueType>, PropertyMapType>
{
	typedef typename find_type<PropertyMapType, KeyType>::value_type holder_type;
	typedef typename holder_type::property_type::value_type target_type;
	typedef typename get_parse_type<ValueType, target_type>::type value_type;
	typedef parse_element<value_type,target_type,typename holder_type::property_type> parser_type;

	template<typename PropertyMapWrapper>
	static void print( void const* object,  details::attribute<KeyType,ValueType> const& e, PropertyMapWrapper & wrapper, std::ostream & out )
	{
		writer_details::wrap_properties<holder_type, ValueType> *used_property = 
			static_cast<writer_details::wrap_properties<holder_type, ValueType>*> ( &wrapper );
/*writer_details::wrap_properties<
			typename find_type<PropertyMapType,KeyType>::value_type::property_type, ValueType
				> *used_property  = &wrapper;*/

		out << ' ' << e.get_name() << "=\"";
		parser_type::write( used_property->get_current( object ) , out  );
		out << '\"';
		used_property->release_current( object );
	}

	
	static void dispatch( Parser* p, details::attribute<KeyType,ValueType> const& e, PropertyMapType const& map, std::vector<bool> & atts_used, char const ** atts )
	{
		char const **ptr = atts;
		while( *ptr )
		{
			if( *ptr == e.get_name() )
			{
				atts_used[ (ptr - atts)>>1 ] = true;

				++ptr;

				parser_type::parse( *ptr, map.template get<KeyType>(), p->get_top().object );
			
				return;
			}
			ptr += 2;
		}
	}
};

template <typename ValueType, typename PropertyMapType>
struct attribute_dispatcher<details::attribute<boost::mpl::void_,ValueType>, PropertyMapType>
{
	template<typename PropertyMapWrapper>
	static void print( void const* object,  details::attribute<boost::mpl::void_,ValueType> const& e, PropertyMapWrapper & wrapper, std::ostream & out )
	{
		out << ' ' << e.get_name() << "=\"\"";
	}

	static void dispatch( Parser* p, details::attribute<boost::mpl::void_,ValueType> const& e, PropertyMapType const& map, std::vector<bool> & atts_used, char const ** atts )
	{
		char const **ptr = atts;
		while( *ptr )
		{
			if( *ptr == e.get_name() )
			{
				atts_used[ (ptr - atts)>>1 ] = true;
				// send that thing to the parser 
				return;
			}
			ptr += 2;
		}
	}
};


template <typename LeftType, typename RightType, typename PropertyMapType>
struct attribute_dispatcher<details::or_combiner<LeftType,RightType>, PropertyMapType>
{
	template<typename PropertyMapWrapper>
	static void print( void const* object,  details::or_combiner<LeftType,RightType> const& e, PropertyMapWrapper & wrapper, std::ostream & out )
	{
		attribute_dispatcher<LeftType,PropertyMapType>::print( object,e.get_left(), wrapper, out );
		attribute_dispatcher<RightType,PropertyMapType>::print( object,e.get_right(), wrapper,  out );
	}

	static void dispatch( Parser* p, details::or_combiner<LeftType,RightType> const& e, PropertyMapType const&map,  std::vector<bool> & atts_used, char const ** atts )
	{
		attribute_dispatcher<LeftType,PropertyMapType>::dispatch(p,e.get_left(), map, atts_used, atts );
		attribute_dispatcher<RightType,PropertyMapType>::dispatch(p,e.get_right(), map, atts_used, atts );
		
		// dont do anything .. the caller has to check whether all attributes have been handled
	}
};


// If valuetype is defined you might act different here..
template <typename KeyType, typename ValueType, typename PropertyMapType>
struct character_dispatcher<details::character_index<KeyType,ValueType>, PropertyMapType >
{

	typedef typename find_type<PropertyMapType, KeyType>::value_type holder_type;
	typedef typename holder_type::property_type::value_type target_type;
	typedef typename get_parse_type<ValueType, target_type>::type value_type;
	typedef parse_element<value_type,target_type,typename holder_type::property_type> parser_type;
	
	template <typename PropertyMapWrapper>
	static void print( void const* object, details::character_index<KeyType,ValueType> const& e, PropertyMapWrapper & wrapper, std::ostream & out )
	{
		writer_details::wrap_properties<holder_type, ValueType> *used_property = 
			static_cast<writer_details::wrap_properties<holder_type, ValueType>*> ( &wrapper );
/*writer_details::wrap_properties<
			typename find_type<PropertyMapType,KeyType>::value_type::property_type, ValueType
				> *used_property  = &wrapper;*/

		parser_type::write( used_property->get_current( object ) , out  );
		used_property->release_current( object );

	}

	static bool dispatch( Parser* p, details::character_index<KeyType,ValueType> const& e, PropertyMapType const& map, std::string const& data )
	{
		DEBUG_COUT("character_dispatcher::dispatch( parser = " << p << ", path_object = " << &e << ", map = "  << &map << ", data = " << data << std::endl );
		parser_type::parse( data, map.template get<KeyType>(), p->get_top().object );
		return true;
	}
};

template <typename ValueType, typename PropertyMapType>
struct character_dispatcher<details::character_index<boost::mpl::void_,ValueType>, PropertyMapType >
{

	template <typename PropertyMapWrapper>
	static void print( void const* object, details::character_index<boost::mpl::void_,ValueType> const& e, PropertyMapWrapper & wrapper, std::ostream & out )
	{
	}
	
	static bool dispatch( Parser* p, details::character_index<boost::mpl::void_,ValueType> const& e, PropertyMapType const& map, std::string const& data )
	{
		DEBUG_COUT("character_dispatcher::dispatch_no_receiver_found( parser = " << p << ", path_object = " << &e << ", map = "  << &map << ", data = " << data << std::endl );
		return true;
	}
};


template <typename PropertyMapType>
struct character_dispatcher<boost::mpl::void_, PropertyMapType >
{
	template <typename PropertyMapWrapper>
	static void print( void const* object, boost::mpl::void_ const& e, PropertyMapWrapper & wrapper, std::ostream & out )
	{
	}
	
	static bool dispatch( Parser* p, boost::mpl::void_  const& e, PropertyMapType const& map, std::string const& data )
	{
		DEBUG_COUT("character_dispatcher::dispatch_no_characters_expected( parser = " << p << ", path_object = " << &e << ", map = "  << &map << ", data = " << data << std::endl );
		// The caller decides what to do .. 
		return false;
	}
};

/*
 * Bekommt Objekt und Pfadelement?
 * 
 */
template <typename PathElement, typename PropertyMapType>
struct ObjectReadNode : public Receiver 
{
	PathElement path;
	PropertyMapType const &map;
	
	ObjectReadNode( PathElement const & p, PropertyMapType const& m) 
		: path( p ), map( m )
	{}

	void print( void const* object, std::ostream & out, std::string const& indent )
	{
		wrap_properties<PathElement, PropertyMapType> wrapper( map );
		wrapper.init( object );

		out << indent << "<" << path.get_name();
		attribute_dispatcher<typename PathElement::attrib_type, PropertyMapType>::print( object, path.get_attributes(), wrapper, out );
		out << ">\n";
		subtag_dispatcher<typename PathElement::subtags_type, PropertyMapType>::print( object, path.get_subtags(), map, indent + "  ", out );
		character_dispatcher<typename PathElement::characters_type, PropertyMapType>::print( object, path.get_characters(), wrapper, out );
		out << indent << "</" << path.get_name() << ">\n"; 
	}
	
	bool new_subtag( Parser *parser, char const * name )
	{
		DEBUG_COUT("ObjectReadNode::new_subtag( parser = " << parser << ", name = " << name << " );" << std::endl);
		return subtag_dispatcher<typename PathElement::subtags_type, PropertyMapType>
			::dispatch( parser, path.get_subtags(), name, map );
	}
	
	void attributes( Parser *parser, char const ** atts, std::vector<bool>& atts_used )
	{
		DEBUG_COUT("ObjectReadNode::attributes( parser = " << parser << " );" << std::endl);
		
		attribute_dispatcher<typename PathElement::attrib_type, PropertyMapType>
			::dispatch( parser, path.get_attributes(), map, atts_used, atts);

	}
	
	bool set_char_data( Parser *parser, std::string const& data )
	{
		DEBUG_COUT("ObjectReadNode::set_char_data( parser = " << parser <<  ", data = " << data << " );" << std::endl);
		return character_dispatcher<typename PathElement::characters_type, PropertyMapType> 
			::dispatch( parser, path.get_characters(), map, data );
	}

	void end_tag( Parser *parser )
	{
		DEBUG_COUT("ObjectReadNode::end_tag( parser = " << parser  << " );"<< std::endl);
		parser->pop_tag();
	}
};

template <typename PathElement>
struct RootReadNode : public Receiver 
{

	PathElement path;
	bool first_call;
	
	RootReadNode( PathElement const & p ) 
		: path( p ), first_call( true )
	{}
	
	bool new_subtag( Parser *parser, char const * name )
	{
		DEBUG_COUT("RootReadNode::new_subtag( parser = " << parser << ", name = " << name << " );" << std::endl);
		if( first_call )
		{	
			DEBUG_COUT("RootReadNode::new_subtag - first_call" << std::endl);
			// gets its own subtag in the first call
			// then attributes are handled 
			if( path.get_name() == name /* || namespace remove */)
			{
				first_call = false;
				return true;
			}
			return false;
		}
		else
		{
			DEBUG_COUT("RootReadNode::new_subtag - following_call" << std::endl);
			// all following calls will be dispatched 
			return subtag_dispatcher<typename PathElement::subtags_type,typename PathElement::map_type>
				::dispatch( parser, path.get_subtags(), name, path.get_map() );
		}
	}
	
	void attributes( Parser *parser, char const ** atts, std::vector<bool>& atts_used )
	{
		attribute_dispatcher<typename PathElement::attrib_type,typename PathElement::map_type>
			::dispatch( parser, path.get_attributes(), path.get_map(), atts_used, atts);
	}

	virtual void print( void const* object, std::ostream & out, std::string const& indent = "")
	{
		wrap_properties<PathElement, typename PathElement::map_type> wrapper( path.get_map() );
		wrapper.init( object );
		out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		out << "<" << path.get_name();
		attribute_dispatcher<typename PathElement::attrib_type, typename PathElement::map_type>::print( object, path.get_attributes(), wrapper, out );
		out << ">\n";
		subtag_dispatcher<typename PathElement::subtags_type, typename PathElement::map_type>::print( object, path.get_subtags(), path.get_map(), "  ", out );
		character_dispatcher<typename PathElement::characters_type, typename PathElement::map_type>::print( object, path.get_characters(), wrapper, out );
		out << "</" << path.get_name() << ">" << std::endl; // should be the only endl
	}
	
	bool set_char_data( Parser *parser, std::string const& data)
	{
		DEBUG_COUT("RootReadNode::set_char_data( parser = " << parser << ", data = " << data << " );" << std::endl);
		return character_dispatcher<typename PathElement::characters_type,typename PathElement::map_type>
			::dispatch( parser, path.get_characters(), path.get_map(), data );
	}
	void end_tag( Parser *parser )
	{
		DEBUG_COUT("RootReadNode::end_tag( parser = " << parser << " );" << std::endl);
		parser->pop_tag();
	}
};


template<typename PathElement, typename PropertyMap>
boost::shared_ptr<Receiver> gen_object_node( PathElement const & pe, PropertyMap const& map )
{
	return boost::shared_ptr<Receiver>( new ObjectReadNode<PathElement, PropertyMap>( pe , map) );
}

template<typename PathElement>
boost::shared_ptr<Receiver> gen_root_node( PathElement const & pe)
{
	return boost::shared_ptr<Receiver>( new RootReadNode<PathElement>( pe ) );
}


}

#endif

