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

#include "xml_exception.h"

xml::ParserError::ParserError( 
		std::string const& t,
		std::string const& f,
		std::string const& p,
		std::string const& c
		) 
: std::runtime_error( t + ":\n" + f + ":" + p + " in \n" + c),
	type(t), file(f), position(p), context(c)
{}

xml::ParserError::~ParserError() throw()
{}

std::string const& xml::ParserError::get_type() const
{
	return type;
}

std::string const& xml::ParserError::get_file() const
{
	return file;
}

std::string const& xml::ParserError::get_position() const
{
	return position;
}

std::string const& xml::ParserError::get_context() const
{
	return context;
}

#define ADD_EXCEPTION_IMPL(A) \
xml::A::A( std::string const& f, std::string const& p, std::string const& c )\
	: ParserError( # A ,f ,p, c ) \
{}\
/*\
xml::A::~A() throw()\
{} \*/
 
ADD_EXCEPTION_IMPL(UnexpectedElement)
ADD_EXCEPTION_IMPL(UnexpectedAttribute)
ADD_EXCEPTION_IMPL(UnexpectedCharacters)
ADD_EXCEPTION_IMPL(ElementParseError)
ADD_EXCEPTION_IMPL(NoMemory)
ADD_EXCEPTION_IMPL(SyntaxError)
ADD_EXCEPTION_IMPL(TagMismatch)
ADD_EXCEPTION_IMPL(InvalidToken)
ADD_EXCEPTION_IMPL(UnclosedToken)
ADD_EXCEPTION_IMPL(UnknownEncoding)
ADD_EXCEPTION_IMPL(IncorrectEncoding)
ADD_EXCEPTION_IMPL(DuplicateAttribute)
ADD_EXCEPTION_IMPL(UnclosedCDATA)
ADD_EXCEPTION_IMPL(UnknownExcpetionCaught)

#undef ADD_EXCEPTION_IMPL

xml::ExceptionCaught::ExceptionCaught( std::string const& file, std::string const& msg )
	: ParserError( "ExceptionCaught", file, " ", msg )
{
}

xml::ExceptionCaught::~ExceptionCaught() throw()
{
}

xml::FileError::FileError( std::string const& file )
	: ParserError( "FileError", file, "", "" )
{
}

xml::FileError::~FileError() throw()
{
}


