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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               *
 ***************************************************************************/
#ifndef TRANSFER_TEST
#include "transfer.h"
#include "wget.h"
#include "curl.h"
#include "config.h"
#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/bind.hpp>
#include "../serialize/debug.h"


xeta::FileTransfer::FileTransfer( std::string const& u, MD5::Sum const& chk )
	: url( u ), checksum( chk ), state( NotStarted )
{
}

void xeta::FileTransfer::set_error( std::string const& message )
{
	state = Error;
	error = message;
}

void xeta::FileTransfer::set_state( xeta::FileTransfer::Status s )
{
	if( state == Error )
		error.clear();
	state = s;
}

xeta::FileTransfer::Status xeta::FileTransfer::get_state() const
{
	return state;
}

std::string const& xeta::FileTransfer::get_local_filename() const
{
	return local_file;
}

std::string const& xeta::FileTransfer::get_directory() const
{
	return directory;
}
xeta::MD5::Sum const& xeta::FileTransfer::get_checksum() const
{
	return checksum;
}
std::string const& xeta::FileTransfer::get_url() const
{
	return url;
}
std::string const& xeta::FileTransfer::get_error() const
{
	return error;
}

bool xeta::FileTransfer::test_chksum() const
{
	MD5 tester;
	MD5::Sum result;
	int i = 0;
	while( true )
	{
		++i;
		if( tester.file( local_file.c_str(), result) )
			break;
		if( i == 5 )
			return false;
	}
	
	return result == checksum;
}

void xeta::FileTransfer::remove_file()
{
	boost::filesystem::remove( boost::filesystem::path( local_file ) );
}

xeta::FileTransfer::~FileTransfer()
{

}



xeta::TransferThread::TransferThread() : worker(0 )
{
}

xeta::TransferThread::~TransferThread()
{
	if( worker )
	{
		worker->join();
		delete worker;
	}
}

void xeta::TransferThread::add_download( xeta::FileTransferPtr item )
{
	lock jobs_access( monitor );
	if( jobs.empty() )
	{
		if( worker) 
		{
			worker->join();
			delete worker;
		}
		worker = new boost::thread( boost::bind( &xeta::TransferThread::run, this ) );
	}
	jobs.push_back( item );
	
}

void xeta::TransferThread::run()
{
	while( true )
	{
		FileTransferPtr current;
		{
			lock jobs_access( monitor );
			if( jobs.empty() )
				return;
			else
				current = jobs.front();
		}

		for( int i = 0; i < 3; ++i )
		{
			current->perform();
			// Test if file exists
			if( !current->test_chksum() )
				current->remove_file();
			else 
				break;
		}

		{
			lock jobs_access( monitor );
			jobs.pop_front();
			if( jobs.empty() )
				return;
		}
	}
}

xeta::FileTransferManager::FileTransferManager()
{
	update_config();
}
xeta::FileTransferManager& xeta::FileTransferManager::get_instance()
{
	static FileTransferManager instance;
	return instance;
}

xeta::FileTransferPtr xeta::FileTransferManager::download( std::string const& url, xeta::MD5::Sum const& checksum )
{
	DEBUG_COUT("xeta::FileTransferManager::download" << std::endl);
	using namespace boost::filesystem;
	using namespace std;
	// Get base directory for downloads
	Config config;
	string distdir =  config["DISTDIR"];
	DEBUG_COUT("DISTDIR=" << distdir << std::endl);
	if( distdir == "" )
	{
		distdir = "/usr/xein/distfiles/";
	}
	DEBUG_COUT("DISTDIR=" << distdir << std::endl);

	path dist_path(distdir, native);
	
	if( !exists( dist_path ) )
		create_directory( dist_path );
	if( !is_directory( dist_path ) )
		throw runtime_error(("path is no directory :" + dist_path.native_directory_string() ).c_str());

	
	DEBUG_COUT("Path - check" << std::endl);
	// get filename
	string::size_type last_slash = url.find_last_of( '/' );
	if( last_slash == string::npos )
		throw runtime_error(("bad url :" +url).c_str());
	
	string filename = url.substr( last_slash + 1 );
	DEBUG_COUT("filename: " << filename <<std::endl);

	// does file exist?
	// if( checksum != check(file) )
	//    remove file and proceed
	// else 
	//    return FileTransferPtr with "good" state


	// get protocol and choost downloader
	string::size_type prot_end = url.find("://");
	
	string protocol = "http";
	if(prot_end != string::npos )
	{
		protocol = url.substr( 0, prot_end );
	}
	DEBUG_COUT("protocol: " << protocol <<std::endl);

	map<string,string>::iterator it = protocols.find(protocol);
	if( it != protocols.end() )
	{
		DEBUG_COUT("protocol found : " << it->first <<std::endl);
		FileTransferPtr new_transfer;
		if( it->second == "curl")
			new_transfer = FileTransferPtr( new Curl( url, checksum ) );
		else // if( it->second == "wget")
			new_transfer = FileTransferPtr( new WGet( url, checksum ) );

		worker.add_download( new_transfer );
		return new_transfer;
	}
	else 
	{
		DEBUG_COUT("no protocol found" <<std::endl);
		throw "could not find protocol";
	}
}

void xeta::FileTransferManager::update_config()
{
	// Static Init TODO : Read from config file 
	protocols["http"] = "curl";
	protocols["https"] = "curl";
	protocols["ftp"] = "curl";
	// read config from 
}


#else
#include "transfer.h"
#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem/exception.hpp>
#include <iostream>

int main ()
{
	try {
	using namespace xeta;
	
	std::string zynot_gif = "3a2eca50d87f5ad40f10bf9cbe70a46f";
	MD5::Sum zyn, ct_logo, heise, ix_logo, tr_logo, hometop;
	xml::read_from_string( zynot_gif, zyn );
	xml::read_from_string("96270d292602be66076b4fb39eb08c6f", ct_logo );
	xml::read_from_string("adf388da04e03bf2059a14b15750a305", heise );
	xml::read_from_string("b6a77316f6229f042141f1c8f5e150e5", hometop);
	xml::read_from_string("34ad4d8f01891e665c3e4b5802527d5e", ix_logo);
	xml::read_from_string("56c14086ddd300c346fc50cf6a1bd6ff", tr_logo);
	
	FileTransferPtr object1 = FileTransferManager::get_instance().download("http://svn.zynot.org/svn/zynot/www/www.dev.zynot.org/trunk/gfx/zynot.gif", zyn );
	FileTransferPtr object2 = FileTransferManager::get_instance().download("http://www.heise.de/icons/ho/heise.gif",heise );
	FileTransferPtr object3 = FileTransferManager::get_instance().download("http://www.heise.de/ct/icons/ct_logo.gif", ct_logo);
	FileTransferPtr object4 = FileTransferManager::get_instance().download("http://www.heise.de/ix/images/2000/ix_logo_w.gif", ix_logo );
	FileTransferPtr object5 = FileTransferManager::get_instance().download("http://www.heise.de/tr/icons/tr_logo.gif", tr_logo);
	FileTransferPtr object6 = FileTransferManager::get_instance().download("http://www.heise.de/tp/pict/hometop.gif", hometop );

	boost::xtime xt;
	boost::xtime_get(&xt, boost::TIME_UTC);
	xt.sec += 30;

	boost::thread::sleep(xt);
	}
	catch(boost::filesystem::filesystem_error &e )
	{
		std::cout << e.what() << std::endl;
	}
	catch(std::exception &e )
	{
		std::cout << e.what() << std::endl;
	}
	catch(...)
	{
		std::cout << "Unknown exception" << std::endl;
	}
}
#endif
