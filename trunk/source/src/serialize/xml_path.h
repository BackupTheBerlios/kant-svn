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
#ifndef XETA_XML_PATH_H
#define XETA_XML_PATH_H
#include <boost/mpl/void.hpp>
#include "xml_parser.h"
namespace xml
{

namespace details 
{
	template <typename KeyType, typename ValueType = boost::mpl::void_>
	struct character_index
	{
		typedef KeyType key_type;
		typedef ValueType value_type;
	};

	template <class LeftT, class RightT>
	class or_combiner
	{
		public:
			typedef LeftT left_type;
			typedef RightT right_type;
		private:
			left_type left_obj_;
			right_type right_obj_;
		public:
			or_combiner( const left_type & left, const right_type & right) 
				: left_obj_(left), right_obj_(right) {}

			inline const left_type & get_left() 	const { return left_obj_;}
			inline const right_type& get_right() 	const { return right_obj_;}

			template <class AnotherRight>
			inline or_combiner< or_combiner< left_type, right_type>, AnotherRight> 
			operator|( const AnotherRight & right ) const
			{
				return or_combiner< or_combiner< left_type, right_type>, AnotherRight> ( *this, right );
			}
			
	};

	template <typename PropertyMap = boost::mpl::void_, 
		typename KeyType = boost::mpl::void_, 
		typename Attributes = boost::mpl::void_, 
		typename SubTags = boost::mpl::void_, 
		typename Characters = boost::mpl::void_>
	class tag;


	template <typename KeyType>
	class predef_tag
	{
		public:
			typedef KeyType key_type;
			typedef predef_tag<key_type> self_type;

		private:
			boost::shared_ptr<Receiver>& obj_node_;
			std::string name;
		public:
			predef_tag( boost::shared_ptr<Receiver>& obj, std::string const& n )
				:  obj_node_(obj), name( n )
			{}

			inline boost::shared_ptr<Receiver> get_object_node() const { return obj_node_;}
			inline std::string const& get_name()  const { return name;}

			template<typename LeftT, typename RightT>
			inline or_combiner<self_type, or_combiner<LeftT, RightT> >
			operator | ( const or_combiner<LeftT, RightT> & right ) const
			{
				return or_combiner<self_type, or_combiner<LeftT, RightT> >(*this, right);
			}

			template <typename OtherPropertyMap, 
					typename OtherKeyType, 
					typename OtherAttibType,
					typename OtherSubTagType, 
					typename OtherCharacterType> 
			inline or_combiner<self_type,
					tag<OtherPropertyMap, 
						OtherKeyType, 
						OtherAttibType, 
						OtherSubTagType, 
						OtherCharacterType> 
					>
			operator | ( const tag<OtherPropertyMap, 
					OtherKeyType, 
					OtherAttibType, 
					OtherSubTagType, 
					OtherCharacterType> & right ) const
			{
				return or_combiner<self_type, tag<OtherPropertyMap, 
						OtherKeyType, 
						OtherAttibType, 
						OtherSubTagType, 
						OtherCharacterType> >(*this, right);
			}

			template <typename OtherKeyType>
			inline or_combiner<
				self_type,
				predef_tag<OtherKeyType>
					>
			operator | ( const predef_tag< OtherKeyType> & right ) const
			{
				return or_combiner<
					self_type,
					predef_tag<OtherKeyType>
					>(*this, right);
			}


	};

	template <typename PropertyMap, 
		typename KeyType, 
		typename Attributes, 
		typename SubTags, 
		typename Characters>
	class tag
	{
		public:
			/**
			 * Type of the object that gets parsed by this class
			 */
			typedef PropertyMap map_type;
			typedef Attributes attrib_type;
			typedef SubTags subtags_type;
			typedef Characters characters_type;
			typedef KeyType key_type;
			
			typedef tag<map_type,KeyType,attrib_type,subtags_type,characters_type> self_type;
			
		private:
			map_type const& map_;
			std::string name_;
			attrib_type attribs_;
			subtags_type subtags_;
			characters_type characters_;
			
		public:
			tag( map_type const& map ) : map_(map) {}
			tag( map_type const& map, std::string const& name) : map_(map), name_(name) {}
			tag( map_type const& map,
					const std::string & name, 
					const attrib_type & attribs, 
					const subtags_type & subtags, 
					const characters_type & characters ) 
				: map_(map), name_(name), attribs_(attribs), 
				subtags_(subtags),  characters_(characters)
			{}

			inline map_type const& 	get_map() 					const { return map_; }
			inline std::string const& 	get_name() 			const { return name_; }
			inline attrib_type const& 	get_attributes() 	const { return attribs_; };
			inline subtags_type const& 	get_subtags() 		const { return subtags_; };
			inline characters_type const& get_characters() 	const { return characters_; };
			
			template <class NewPropertyMap>
			inline tag<NewPropertyMap,key_type,attrib_type,subtags_type,characters_type>
			set_type( map_type const& map, std::string name ) const
			{
				return tag<NewPropertyMap,key_type,attrib_type,subtags_type,characters_type>
					( map, name, attribs_,subtags_,characters_);
			}

			template <typename NewKeyType>
			inline tag<map_type, NewKeyType, attrib_type, subtags_type, characters_type>
			assign( const std::string & name ) const
			{
				return tag<map_type, NewKeyType, attrib_type, subtags_type, characters_type>
					( map_, name, attribs_, subtags_,  characters_ );
			}

			template <class NewPropertyMap, typename NewKeyType>
			inline tag<NewPropertyMap, NewKeyType, attrib_type, subtags_type, characters_type>
			assign( map_type& map, const std::string & name ) const
			{
				return tag<map_type, NewKeyType, attrib_type, subtags_type, characters_type>
					( map, name, attribs_, subtags_, characters_ );
			}

			template <class NewAttributes>
			inline tag<map_type,key_type,NewAttributes,subtags_type,characters_type>
			attributes( const NewAttributes & attribs ) const
			{
				return  tag<map_type,key_type,NewAttributes,subtags_type,characters_type>
					( map_, name_, attribs, subtags_, characters_ ); 
			}

			template <class NewSubTags>
			inline tag<map_type,key_type,attrib_type,NewSubTags,characters_type>
			sub_tags( const NewSubTags& tags) const
			{
				return  tag<map_type,key_type,attrib_type,NewSubTags,characters_type>
					( map_, name_, attribs_, tags,  characters_ ); 
			}

			template <typename CharKeyType>
			inline tag<map_type,key_type,attrib_type,subtags_type, character_index< CharKeyType> >
			characters( ) const
			{
				return tag<map_type,key_type,attrib_type,subtags_type,character_index<CharKeyType> >
					( map_, name_, attribs_, subtags_,  character_index<CharKeyType>() ); 
			}

			template <class TemporaryType, typename CharKeyType>
			inline tag<map_type,key_type,attrib_type,subtags_type, character_index<CharKeyType,TemporaryType> >
			characters( ) const
			{
				return tag<map_type,key_type,attrib_type,subtags_type,character_index<CharKeyType, TemporaryType> >
					( map_, name_, attribs_, subtags_,  character_index<CharKeyType,TemporaryType>() ); 
			}


			template <class LeftT, class RightT> 
			inline or_combiner<self_type, or_combiner<LeftT, RightT> >
			operator | ( const or_combiner<LeftT, RightT> & right ) const
			{
				return or_combiner<self_type, or_combiner<LeftT, RightT> >(*this, right);
			}

			template <typename OtherPropertyMap, 
					typename OtherKeyType, 
					typename OtherAttibType,
					typename OtherSubTagType, 
					typename OtherCharacterType> 
			inline or_combiner<self_type,
					tag<OtherPropertyMap, 
						OtherKeyType, 
						OtherAttibType, 
						OtherSubTagType, 
						OtherCharacterType> 
					>
			operator | ( tag<OtherPropertyMap, 
					OtherKeyType, 
					OtherAttibType, 
					OtherSubTagType, 
					OtherCharacterType> const & right ) const
			{
				return or_combiner<self_type, tag<OtherPropertyMap, 
						OtherKeyType, 
						OtherAttibType, 
						OtherSubTagType, 
						OtherCharacterType> >(*this, right);
			}

			template <typename OtherKeyType> 
			inline or_combiner<	self_type, predef_tag<OtherKeyType> >
			operator | ( predef_tag<OtherKeyType> const & right ) const
			{
				return or_combiner<self_type, predef_tag<OtherKeyType>	>(*this, right);
			}

	};

	template <typename KeyType = boost::mpl::void_, typename ValueType = boost::mpl::void_>
	class attribute
	{
		public:
			typedef KeyType key_type;
			typedef ValueType value_type;
			typedef attribute<key_type, value_type> self_type;

		private:
			std::string name_;

		public:
			attribute( ) {}
			attribute( const std::string name ) : name_( name ) {}

			inline const std::string & 	get_name() 			const {  return name_; }

			template <typename NewKeyType>
			inline attribute<NewKeyType, value_type>
			assign( const std::string & name ) const
			{
				return attribute<NewKeyType, value_type>( name );
			}

			template <typename NewKeyType, typename NewValueType>
			inline attribute<NewKeyType, NewValueType>
			assign( const std::string & name ) const
			{
				return attribute<NewKeyType, NewValueType>( name );
			}

			template <typename LeftT, typename RightT> 
			inline or_combiner<self_type, or_combiner<LeftT, RightT> >
			operator | ( const or_combiner<LeftT, RightT> & right ) const
			{
				return or_combiner<self_type, or_combiner<LeftT, RightT> >(*this, right);
			}

			template <typename OtherKeyType, typename OtherValueType> 
			inline or_combiner<self_type, attribute<OtherKeyType, OtherValueType> >
			operator | ( const attribute<OtherKeyType, OtherValueType>& right ) const
			{
				return or_combiner<self_type, attribute<OtherKeyType, OtherValueType> >(*this, right);
			}

	};

}

const details::tag<> tag( boost::mpl::void_());
const details::attribute<> attribute;
	
template<typename KeyType, typename PropertyMapType>
const details::tag< PropertyMapType, KeyType>
sub_tag( PropertyMapType const& map, const std::string& name) 
{
	return details::tag<PropertyMapType,KeyType>(map,name);
}

const details::tag< boost::mpl::void_, boost::mpl::void_>
no_obj_tag( const std::string& name) 
{
	return details::tag<boost::mpl::void_,boost::mpl::void_>(boost::mpl::void_(),name);
}


template<typename PropertyMapType>
const details::tag<PropertyMapType>
root_tag( PropertyMapType const& map, const std::string& name) 
{
	return details::tag<PropertyMapType>(map,name);
}

template<typename KeyType>
const details::predef_tag<KeyType>
link_tag( boost::shared_ptr<xml::Receiver> & rec, std::string const& name ) 
{
	return details::predef_tag<KeyType>( rec, name );
}


}

#endif
