/* md5.h - Declaration of functions and data types used for MD5 sum
   computing library functions.
   Copyright (C) 1995, 1996, 1999, 2000, 2003 Free Software Foundation, Inc.
   NOTE: The canonical source of this file is maintained with the GNU C
   Library.  Bugs can be reported to bug-glibc@prep.ai.mit.edu.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef XETA_MD5_H
#define XETA_MD5_H

#include <string>
#include <iostream>
//#include <climits>
#include "xeta_types.h"

namespace xeta {
/* Structure to save state of computation between the single steps.  */

class MD5
{
	public:
		struct Sum // TODO
		{
			unsigned char data[16];
		};
	private:
		uint32 A;
		uint32 B;
		uint32 C;
		uint32 D;

		uint32 total[2];
		uint32 buflen;
		char internal_buffer[128];
		void read_ctx( Sum& result );
		void finish_ctx( Sum& result );
		void process_block( char const* words, size_t char_len );
		void process_bytes( char const* buffer, size_t char_len );
		void init();
	public:
		/** Initialize structure containing state of computation.
		   (RFC 1321, 3.3: Step 3)  */
		bool file( const char *filename, Sum& result );
		void buffer( const char* buffer, size_t char_len, Sum& result );
		
};

}
namespace xml {
	void read_from_string( std::string const& data, xeta::MD5::Sum & obj );
	void write_to_string( std::string & data, xeta::MD5::Sum const& obj );
}


bool operator==( xeta::MD5::Sum const& left, xeta::MD5::Sum const & right );
bool operator!=( xeta::MD5::Sum const& left, xeta::MD5::Sum const & right ); 
std::istream& operator>>( std::istream& input, xeta::MD5::Sum & object );
std::ostream& operator<<( std::ostream& input, xeta::MD5::Sum const& object );

#endif
