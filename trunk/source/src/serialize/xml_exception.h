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

#ifndef XETA_XML_EXCEPTION_H
#define XETA_XML_EXCEPTION_H
#include <stdexcept>
#include <string>
namespace xml
{
	class ParserError : public std::runtime_error
	{
		private:
			std::string type, file, position, context;
		public:
			ParserError( std::string const& t, std::string const& f, std::string const& p, std::string const& c );
			std::string const& get_type() const;
			std::string const& get_file() const;
			std::string const& get_position() const;
			std::string const& get_context() const;
			~ParserError() throw();
	};
	class UnexpectedElement : public ParserError{
		public:
			UnexpectedElement( std::string const& f, std::string const& p, std::string const& c );
	};
	class UnexpectedAttribute : public ParserError {
		public:
			UnexpectedAttribute( std::string const& f, std::string const& p, std::string const& c );
	};
	class ElementParseError : public ParserError {
		public:
			ElementParseError( std::string const& f, std::string const& p, std::string const& c );
	};
	class UnexpectedCharacters : public ParserError {
		public:
			UnexpectedCharacters( std::string const& f, std::string const& p, std::string const& c );
	};
	class NoMemory : public ParserError {
		public:
			NoMemory( std::string const& f, std::string const& p, std::string const& c );
	};
	class SyntaxError : public ParserError {
		public:
			SyntaxError( std::string const& f, std::string const& p, std::string const& c );
	};
	class TagMismatch : public ParserError {
		public:
			TagMismatch( std::string const& f, std::string const& p, std::string const& c );
	};
	class InvalidToken: public ParserError {
		public:
			InvalidToken( std::string const& f, std::string const& p, std::string const& c );
	};
	class UnclosedToken: public ParserError {
		public:
			UnclosedToken( std::string const& f, std::string const& p, std::string const& c );
	};
	class UnknownEncoding : public ParserError {
		public:
			UnknownEncoding( std::string const& f, std::string const& p, std::string const& c );
	};
	class IncorrectEncoding : public ParserError {
		public:
			IncorrectEncoding( std::string const& f, std::string const& p, std::string const& c );
	};
	class DuplicateAttribute : public ParserError {
		public:
			DuplicateAttribute( std::string const& f, std::string const& p, std::string const& c );
	};
	class UnclosedCDATA: public ParserError {
		public:
			UnclosedCDATA( std::string const& f, std::string const& p, std::string const& c );
	};
	class UnknownExcpetionCaught : public ParserError {
		public:
			UnknownExcpetionCaught( std::string const& file, std::string const& p, std::string const& c );
	};

	class ExceptionCaught : public ParserError {
		public:
			ExceptionCaught( std::string const& file, std::string const& msg );
			~ExceptionCaught() throw();
	};
	class FileError : public ParserError {
		public:
			explicit FileError( std::string const& file );
			~FileError() throw();
	};
}
#endif

