/***************************************************************************
 *   Copyright (C) 2004 by Andreas Pokorny <dalbuschat@zynot.org>          *
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
#ifndef XETA_UUID_H
#define XETA_UUID_H

#include "xeta_types.h"
#include <iostream>

namespace xeta
{
struct uuid
{
	public:
		uint32 time_low;
		uint16 time_mid;
		uint16 time_hi_and_version;
		uint16 clock_seq;
		uint8 node[6];
		void generate();
		void to_buffer( unsigned char* buffer) const;
	private:
		void generate_random();
		void generate_time();
		void read_from_buffer( unsigned char const* buffer);
		void write_to_buffer( unsigned char * buffer) const;
		
};

}
namespace xml {
	void read_from_string( std::string const& data, xeta::uuid & obj );
	void write_to_string( std::string & data, xeta::uuid const& obj );
}

std::ostream& operator<<( std::ostream & out, xeta::uuid const& obj );
std::istream& operator>>( std::istream & in, xeta::uuid & obj );
bool operator==( xeta::uuid const& left, xeta::uuid const& right);
bool operator!=( xeta::uuid const& left, xeta::uuid const& right);

#endif

