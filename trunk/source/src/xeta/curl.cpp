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
 * Foundation, Inc., 674 Mass Ave, Cambridge, MA 02139, USA.               *
 ***************************************************************************/

#include "transfer.h"
#include "curl.h" 
#define XETA_PRINT_FUNCTION
#include "../serialize/debug.h"
#include <fstream>
#include <boost/bind.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>
#include <curlpp/curlpp.hpp>


xeta::Curl::Curl( std::string const& url, xeta::MD5::Sum const& checksum  ) 
	: FileTransfer( url,  checksum )
{
	static bool initialized = false;
	if( !initialized )
	{
		curlpp::initialize();
		initialized = true;
	}
}

void xeta::Curl::perform()
{
	static curlpp::easy handle;
	try {
		std::ofstream output( get_local_filename().c_str() );
		handle.url( get_url() );
		curlpp::ostream_trait o_trait( &output );
		handle.m_body_storage.trait( &o_trait );

		set_state( xeta::FileTransfer::Downloading );
		handle.perform();
		set_state( xeta::FileTransfer::Downloaded );
	}
	catch( curlpp::exception & e) 
	{
		set_error( e.what() );
	}

}

