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

#ifndef ZYNOT_XML_PARSER_H
#define ZYNOT_XML_PARSER_H

#include <string>
#include <list>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include "xml_exception.h"
namespace xml
{
	class Parser;
	class Receiver
	{
		private:
			bool throw_on_characters, throw_on_element, throw_on_attribute;
			virtual bool new_subtag( Parser *parser, char const * name ) = 0;
			virtual void attributes( Parser *parser, char const ** atts, std::vector<bool>& atts_used ) = 0;
			virtual bool set_char_data( Parser *parser, std::string const& data ) = 0;
			virtual void end_tag( Parser *parser ) = 0;
		public:
			Receiver();
			void set_throw_on_element( bool toe );
			void set_throw_on_characters( bool toc );
			void set_throw_on_attribute( bool toa );
			bool get_throw_on_element() const;
			bool get_throw_on_characters() const;
			bool get_throw_on_attribute() const;

			virtual void print( void const* object, std::ostream & out, std::string const& indent = "") = 0;
			void receive_new_subtag( Parser *parser, char const* name );
			void receive_attributes( Parser *parser, char const** atts );
			void receive_characters( Parser *parser, std::string const& data );
			void receive_end_tag( Parser *parser );
			virtual ~Receiver();
	};

	class DumbReceiver : public Receiver
	{
		private:
			virtual bool new_subtag( Parser *parser, char const * name );
			virtual void attributes( Parser *parser, char const ** atts, std::vector<bool>& atts_used );
			virtual bool set_char_data( Parser *parser, std::string const& data );
			virtual void end_tag( Parser *parser );
			virtual void print( void const* object, std::ostream & out, std::string const& indent = "");
		public: 
			DumbReceiver();
	};

	
	class Parser
	{
		public:
			enum ErrorStates {
				NoError,
				UnknownExcpetionCaught,
				ExceptionCaught,
				UnexpectedElement,
				UnexpectedAttribute,
				ElementParseError,
				UnexpectedCharacters,
				NoMemory,
				SyntaxError,
				TagMismatch,
				InvalidToken,
				UnclosedToken,
				UnknownEncoding,
				IncorrectEncoding,
				DuplicateAttribute,
				UnclosedCDATA,
				GeneralExpatError
			};
		private:
			ErrorStates error_status;
			std::string file, position, context, type;
			void *xml_parser;
			struct State
			{
				State( boost::shared_ptr<Receiver> r =  boost::shared_ptr<Receiver>(), void *o = 0 );
				~State();
				boost::shared_ptr<Receiver> rec;
				void *object;
				std::string chars;
				boost::function0<void> on_removal;
			};
			std::list<State> stack;

			// Callback methods for expat:
			static void end_element( Parser *parser, const char *name );
			static void start_element( Parser *parser, const char *name, const char ** atts );
			static void char_data( Parser *parser, const char *data, int len );
			void handle_error();
		public:
			Parser();
			size_t get_stack_size() const;
			void set_error( ErrorStates state );
			void push_tag( boost::shared_ptr<Receiver> rec, void* obj );
			void pop_tag();
			State & get_top();

			void parse( boost::shared_ptr<Receiver> root_element, std::string const& filename, void *root_obj );
	};
}

#endif
