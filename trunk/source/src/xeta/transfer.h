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

#ifndef XETA_DOWNLOAD_H
#define XETA_DOWNLOAD_H

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <string>
#include <map>
#include "md5.h"

namespace xeta {

class FileTransfer 
{
	public:
		enum Status
		{
			NotStarted,
			Error, 
			Downloading, 
			Downloaded,
			Checked
		};
	private:
		std::string url, local_file, directory, error;
		MD5::Sum checksum;
		Status state;
	protected:
		void set_error( std::string const& message );
		void set_state( Status state );
	public:
		FileTransfer( std::string const& url, MD5::Sum const& req_chk_sum );
		Status get_state() const;

		std::string const& get_local_filename() const;
		std::string const& get_directory() const;
		MD5::Sum const& get_checksum() const;
		std::string const& get_url() const;
		std::string const& get_error() const;
		
		virtual bool test_chksum() const;
		virtual void perform() = 0;
		virtual void remove_file();

		// maybe add terminate here!
		virtual ~FileTransfer();
};

typedef boost::shared_ptr<FileTransfer> FileTransferPtr;

class TransferThread : public boost::noncopyable
{
	private:
		boost::thread * worker;
		boost::mutex monitor;
		typedef boost::mutex::scoped_lock lock;
		std::list<FileTransferPtr> jobs;
		void run();
	public:
		TransferThread();
		~TransferThread();
		void add_download( FileTransferPtr item );
		// maybe add thread termination
};

class FileTransferManager
{
	public:
		void update_config();
		// TODO: Instead of url checksum, use a larger fileinfo object that can be modified and passed to other
		// subsystems.. 
		// Fileinfo := ( file_size, file_name, checksum, url , ..? )
		FileTransferPtr download( std::string const& url, xeta::MD5::Sum const& checksum );
		static FileTransferManager& get_instance();
		
	private:
		TransferThread worker;
		FileTransferManager();
		FileTransferManager& operator=( FileTransferManager const& cp);
		FileTransferManager( FileTransferManager const& cp);
		
		std::map<std::string, std::string> protocols;
};

}

#endif

