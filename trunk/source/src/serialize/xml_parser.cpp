#include "xml_parser.h"
#define DEBUG
#define XETA_PRINT_FUNCTION
#include "debug.h"
#include <expat.h>
#include <fstream>
#include <sstream>
#include <stdexcept>


xml::Receiver::~Receiver()
{
}

xml::Receiver::Receiver() 
	: throw_on_characters(false), throw_on_element(true), throw_on_attribute(true)
{
}

void xml::Receiver::set_throw_on_element( bool toe )
{
	throw_on_element = toe;
}
void xml::Receiver::set_throw_on_characters( bool toc )
{
	throw_on_characters = toc;
}
void xml::Receiver::set_throw_on_attribute( bool toa )
{
	throw_on_attribute = toa;
}

bool xml::Receiver::get_throw_on_element() const
{
	return throw_on_element;
}
bool xml::Receiver::get_throw_on_characters() const
{
	return throw_on_characters;
}
bool xml::Receiver::get_throw_on_attribute() const
{
	return throw_on_attribute;
}


void xml::Receiver::receive_new_subtag( xml::Parser *parser, char const* name )
{
	size_t stack_size = parser->get_stack_size();
	if( ! this->new_subtag( parser, name ) && throw_on_element )
	{
		DEBUG_COUT(" setting unexpected element" << std::endl );
		parser->set_error( xml::Parser::UnexpectedElement );
	}
	// if the "new_subtag" of our generated parser did not add a 
	// receiver on the stack we have to add a dummy receiver to 
	// avoid problems when this element gets closed
	//
	// Attention - the root node will not add a new subtag, because it receives itself
	else if( stack_size == parser->get_stack_size() && stack_size != 1)
	{
		parser->push_tag(  boost::shared_ptr<Receiver>( new DumbReceiver ), 0 );
	}
}

void xml::Receiver::receive_attributes( Parser *parser, char const** atts )
{
	char const** ptr = atts;
	std::vector<bool> results;
	while( *ptr ) 
		results.push_back(false),ptr+=2;
	
	this->attributes( parser, atts, results );
	// FIXME ~ make that user controllable
	if( throw_on_attribute )
		for( std::vector<bool>::const_iterator it = results.begin(),e = results.end();
				it != e; ++ it )
			if( !*it )
			{
				parser->set_error( xml::Parser::UnexpectedAttribute );
				return;
			}
}

void xml::Receiver::receive_characters( Parser *parser, std::string const& data )
{
	DEBUG_COUT("( parser = " << parser << ", data = " << data << " );" << std::endl );
	if( !set_char_data( parser, data ) && throw_on_characters ) 
		parser->set_error( xml::Parser::UnexpectedCharacters );
}

void xml::Receiver::receive_end_tag( Parser *parser )
{
	DEBUG_COUT("( parser = " << parser << " );" << std::endl );
	end_tag( parser );
}
////////////////////////////////////////////////////////////////////////////

xml::DumbReceiver::DumbReceiver()
{
}

bool xml::DumbReceiver::new_subtag( Parser *parser, char const * name )
{
	parser->push_tag(  parser->get_top().rec, 0 );
	return true;
}

void xml::DumbReceiver::attributes( Parser *parser, char const ** atts, std::vector<bool>& atts_used )
{
	// we do not change the attribute state.
	// maybe we should set all attributes to true?
}

bool xml::DumbReceiver::set_char_data( Parser *parser, std::string const& data )
{
	return true; // thats an unknown object anyway so we should not cause errors in here.
}

void xml::DumbReceiver::end_tag( Parser *parser )
{
	parser->pop_tag();
}

void xml::DumbReceiver::print( void const* object, std::ostream & out, std::string const& indent)
{
}
////////////////////////////////////////////////////////////////////////////

xml::Parser::Parser()
	: error_status( NoError), xml_parser(0)
{
}

size_t xml::Parser::get_stack_size() const
{
	return stack.size();
}

void xml::Parser::set_error( xml::Parser::ErrorStates state )
{
	DEBUG_COUT("( state = " << state << " );" << std::endl );
	if( state == GeneralExpatError )
		type = reinterpret_cast<const char*>(XML_ErrorString( XML_GetErrorCode( static_cast<XML_Parser>(this->xml_parser) ) ) );

	error_status = state;
	std::ostringstream pos;
	pos << " line " << XML_GetCurrentLineNumber( static_cast<XML_Parser>(this->xml_parser) ) 
		<< XML_GetCurrentColumnNumber( static_cast<XML_Parser>(this->xml_parser) ) ;

	int offset, size;
	const char * con = XML_GetInputContext( static_cast<XML_Parser>(this->xml_parser), &offset, &size);
	context.assign( con+offset, con+offset+size );

	position = pos.str();
}

#define CASE_THROW(A) case xml::Parser::A: throw xml::A( file, position, context );break;
void xml::Parser::handle_error()
{
	DEBUG_COUT("() :  this->error_status = " << error_status << " );" << std::endl );
	if( error_status != NoError )
		XML_ParserFree( static_cast<XML_Parser>(xml_parser) );
	switch( error_status )
	{
		CASE_THROW(UnknownExcpetionCaught) 
		CASE_THROW(UnexpectedElement) 
		CASE_THROW(UnexpectedAttribute)
		CASE_THROW(ElementParseError)
		CASE_THROW(UnexpectedCharacters)
		CASE_THROW(NoMemory)
		CASE_THROW(SyntaxError)
		CASE_THROW(TagMismatch)
		CASE_THROW(InvalidToken)
		CASE_THROW(UnclosedToken)
		CASE_THROW(UnknownEncoding)
		CASE_THROW(IncorrectEncoding)
		CASE_THROW(DuplicateAttribute)
		CASE_THROW(UnclosedCDATA)
		case xml::Parser::ExceptionCaught: throw xml::ExceptionCaught(file, context); break;
		case xml::Parser::GeneralExpatError: throw xml::ParserError( type, file, position, context); break;
		case xml::Parser::NoError: break;
		default: break;
	};
}
#undef CASE_THROW 

void xml::Parser::end_element( Parser *parser, const char *name )
{
	if( parser->error_status == NoError )
	{
		try 
		{
			DEBUG_COUT("xml::Parser::end_element( " << parser << ", " << name << std::endl);
			boost::shared_ptr<Receiver> secure_copy = parser->stack.back().rec;
			DEBUG_COUT("\tsetting character data"<<std::endl);
			secure_copy->receive_characters( parser, parser->stack.back().chars );
			boost::shared_ptr<Receiver> sec_secure_copy = parser->stack.back().rec;
			DEBUG_COUT("\tend tag" <<std::endl);
			sec_secure_copy->receive_end_tag( parser );
		} 
		catch( xml::UnexpectedElement &e )
		{ parser->set_error( Parser::UnexpectedElement ); }
		catch( xml::UnexpectedAttribute &e )
		{ parser->set_error( Parser::UnexpectedAttribute); }
		catch( xml::ElementParseError &e )
		{ parser->set_error( Parser::ElementParseError); }
		catch( xml::UnexpectedCharacters &e )
		{ parser->set_error( Parser::UnexpectedCharacters); }
		catch( std::exception & e )
		{ parser->set_error( Parser::ExceptionCaught ); }
		catch( ... )
		{ parser->set_error(Parser::UnknownExcpetionCaught ); }
	}
}
void xml::Parser::start_element( Parser *parser, const char *name, const char ** atts )
{
	if( parser->error_status == NoError )
	{

		try 
		{
			DEBUG_COUT("xml::Parser::start_element( " << parser << ", " << name << std::endl);
#ifdef DEBUG
			const char** ptr = atts;
			while(*ptr)
				std::cout <<"   " << *ptr++ << " = " << *ptr++ << std::endl;
#endif
			DEBUG_COUT( ")" << std::endl);
			boost::shared_ptr<Receiver> secure_copy = parser->stack.back().rec;
			secure_copy->receive_new_subtag( parser, name );
			boost::shared_ptr<Receiver> sec_secure_copy = parser->stack.back().rec;
			sec_secure_copy->receive_attributes( parser, atts );
		}
		catch( xml::UnexpectedElement &e )
		{ parser->set_error( Parser::UnexpectedElement ); }
		catch( xml::UnexpectedAttribute &e )
		{ parser->set_error( Parser::UnexpectedAttribute); }
		catch( xml::ElementParseError &e )
		{ parser->set_error( Parser::ElementParseError); }
		catch( xml::UnexpectedCharacters &e )
		{ parser->set_error( Parser::UnexpectedCharacters); }
		catch( std::exception & e )
		{ parser->set_error( Parser::ExceptionCaught ); }
		catch( ... )
		{ parser->set_error(Parser::UnknownExcpetionCaught ); }
	}
}
void xml::Parser::char_data( Parser *parser, const char *data, int len )
{
	if( parser->error_status == NoError )
	{
		try 
		{
			DEBUG_COUT("xml::Parser::char_data( " << parser << ", " << std::string( data, data + len ) << std::endl);
			parser->stack.back().chars.append( data, data+len ); // TODO: check if that thing works properly
		}
		catch( xml::UnexpectedElement &e )
		{ parser->set_error( Parser::UnexpectedElement ); }
		catch( xml::UnexpectedAttribute &e )
		{ parser->set_error( Parser::UnexpectedAttribute); }
		catch( xml::ElementParseError &e )
		{ parser->set_error( Parser::ElementParseError); }
		catch( xml::UnexpectedCharacters &e )
		{ parser->set_error( Parser::UnexpectedCharacters); }
		catch( std::exception & e )
		{ parser->set_error( Parser::ExceptionCaught ); }
		catch( ... )
		{ parser->set_error(Parser::UnknownExcpetionCaught ); }
	}

}

xml::Parser::State::State( boost::shared_ptr<Receiver> r , void *o )
	: rec(r), object(o)
{
}
xml::Parser::State::~State()
{
	if( on_removal )
		on_removal();
}
void xml::Parser::push_tag( boost::shared_ptr<Receiver> rec, void *o)
{
	DEBUG_COUT("( " << this << ", " << &*rec  << ", " << o << ");" << std::endl);
	
	// We only propagate the receiver behaviour settings if the receiver has not been 
	// configured by the user. There are receiver created within the serialization templates
	// and receivers generated outside and used through the link_tag interface. In the 
	// latter case tag-based user configuration is possible. 
	if( rec->get_throw_on_attribute() && !rec->get_throw_on_characters() && rec->get_throw_on_element() )
	{
		rec->set_throw_on_attribute( stack.back().rec->get_throw_on_attribute() );
		rec->set_throw_on_element( stack.back().rec->get_throw_on_element() );
		rec->set_throw_on_characters( stack.back().rec->get_throw_on_characters() );
	}
	stack.push_back( State(rec,o) );
}

xml::Parser::State & xml::Parser::get_top()
{
	return stack.back();
}

void xml::Parser::pop_tag()
{
	DEBUG_COUT("( " << this << ", " << &*(stack.back().rec) << ");" << std::endl);
	return stack.pop_back();
}


void xml::Parser::parse( boost::shared_ptr<Receiver> root_element, std::string const& filename, void *root_obj )
{
	error_status = NoError;
	type.clear();
	position.clear();
	context.clear();
	file = filename;

	std::ifstream input( filename.c_str() );

	if( ! input.is_open() )
		throw FileError(filename);

	XML_Parser p = XML_ParserCreate( "UTF-8" );
	std::cout << " P === " << p << std::endl;
	xml_parser = p;
	std::cout << " xml PARSER === " << xml_parser << std::endl;

	stack.clear();
	stack.push_back( State( root_element,root_obj ) );

	XML_SetUserData( p, this);

	XML_SetElementHandler( 
			p, 
			reinterpret_cast<XML_StartElementHandler>( &Parser::start_element ), 
			reinterpret_cast<XML_EndElementHandler>( &Parser::end_element )
			);
	XML_SetCharacterDataHandler( p,	reinterpret_cast<XML_CharacterDataHandler>( &Parser::char_data ) );

	char * buffer = reinterpret_cast<char*>( XML_GetBuffer(p, 4000 ) );
	int len = 0;
	while( (len = input.readsome( buffer, 4000 ) ) )
	{
		DEBUG_COUT("len : " << len << std::endl);
		if( XML_ParseBuffer( p, len,  (len != 4000)  ) == XML_STATUS_ERROR );
		{
			enum XML_Error err = XML_GetErrorCode( p );
			switch( err )
			{
				case XML_ERROR_NONE:break;
				case XML_ERROR_NO_MEMORY: set_error(Parser::NoMemory);  break;
				case XML_ERROR_SYNTAX:  set_error(Parser::SyntaxError);  break;
				case XML_ERROR_INVALID_TOKEN: set_error(Parser::InvalidToken);  break;
				case XML_ERROR_TAG_MISMATCH: set_error(Parser::TagMismatch ); break;
				case XML_ERROR_DUPLICATE_ATTRIBUTE: set_error(Parser::DuplicateAttribute); break;
				case XML_ERROR_UNKNOWN_ENCODING: set_error( Parser::UnknownEncoding ); break;
				case XML_ERROR_INCORRECT_ENCODING: set_error( Parser::IncorrectEncoding ); break;
				case XML_ERROR_UNCLOSED_CDATA_SECTION: set_error( Parser::UnclosedCDATA ); break;
				// All other errors:
				case XML_ERROR_NO_ELEMENTS:
				case XML_ERROR_UNCLOSED_TOKEN:
				case XML_ERROR_PARTIAL_CHAR:
				case XML_ERROR_JUNK_AFTER_DOC_ELEMENT:
				case XML_ERROR_PARAM_ENTITY_REF:
				case XML_ERROR_UNDEFINED_ENTITY:
				case XML_ERROR_RECURSIVE_ENTITY_REF:
				case XML_ERROR_ASYNC_ENTITY:
				case XML_ERROR_BAD_CHAR_REF:
				case XML_ERROR_BINARY_ENTITY_REF:
				case XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF:
				case XML_ERROR_MISPLACED_XML_PI:
				case XML_ERROR_EXTERNAL_ENTITY_HANDLING:
				case XML_ERROR_NOT_STANDALONE:
				case XML_ERROR_UNEXPECTED_STATE:
				case XML_ERROR_ENTITY_DECLARED_IN_PE:
				case XML_ERROR_FEATURE_REQUIRES_XML_DTD:
				case XML_ERROR_CANT_CHANGE_FEATURE_ONCE_PARSING:
				case XML_ERROR_UNBOUND_PREFIX:
						set_error( xml::Parser::GeneralExpatError);
						break;
				default: break;
			}
			if( err != XML_ERROR_NONE || error_status != NoError)
				handle_error();
		}
		buffer = reinterpret_cast<char*>( XML_GetBuffer(p, 4000 ) );
	}
	XML_ParserFree( p );
}

