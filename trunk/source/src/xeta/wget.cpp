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

#define DEBUG
#include "transfer.h"
#include "wget.h"
#include "../serialize/debug.h"
#include <fcntl.h>
#include <signal.h>
#include <cstdlib>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

xeta::WGet::WGet( std::string const& url, xeta::MD5::Sum const& checksum  ) 
	: FileTransfer( url,  checksum ), wget_pid( -1 )
{
}

void xeta::WGet::perform( )
{
	pid_t pid;
	int pipe_fd[2];

	char destination[512];
	memset( destination, 0, 512);
	char url[1024];
	memset( url, 0, 1024);

	strncpy( destination, get_directory().c_str(), 511);
	strncpy( url, get_url().c_str(), 1023);

	DEBUG_COUT("URL :" << url << "|" <<std::endl);
	DEBUG_COUT("Destination :" << destination << "|" <<std::endl);
	DEBUG_COUT("Destination :" << get_directory() << "|" <<std::endl);

	
	if(pipe( pipe_fd ) < 0)
	{
		set_error( "Could not create pipe" );
		// error
		return;
	}
	
	pid = fork();

	if( pid == 0) 
	{
		// Build call
		// generate args
		// check for params in config
		close(pipe_fd[0]);	
		dup2( pipe_fd[1], 2 );

		putenv ("LC_ALL=C");
		execlp( "wget", "wget", "-v", "-P", destination, url, "-c", "-t", "5",  0 );
#define TEST(A) << #A << "==errno :" << ( A==errno ) << std::endl
		std::cerr << "OOPS: " << std::endl TEST(EACCES)  TEST(EPERM) 
			TEST(E2BIG) TEST(ENOEXEC) TEST(EFAULT) TEST(ENAMETOOLONG) 
			TEST(ENOENT) TEST(ENOMEM) TEST(ENOTDIR) TEST(ELOOP) TEST(ETXTBSY)
			TEST(EIO) TEST(ENFILE) TEST(EMFILE) TEST(EINVAL) TEST(EISDIR) TEST(ELIBBAD);
#undef TEST
		_exit (255);
	}
	if( pid < 0)
	{
		set_error( "Could not fork." );
		// ERROR
		return;
	}

	close( pipe_fd[1]);
	wget_pid = pid;
}

xeta::WGet::~WGet()
{
}




/*void xeta::WGet::update_state()
{
	// Process wget_log messages 
	parse_output();

	// Check that wget process is still running 
	int status;
	pid_t child_pid = waitpid ( wget_pid, &status, WNOHANG | WUNTRACED);
	if (child_pid == wget_pid) 
	{
		// Wget process stopped so we must register its exit 
		close( log_fd );


		if( WIFEXITED( status ) )
		{
			if(WIFEXITED(status) == 0)
			{
				jobs.front()->set_state(xeta::FileTransfer::Downloaded);
			}
			else if (WIFEXITED(status) == 255 )
			{
				jobs.front()->set_state(xeta::FileTransfer::Error);
			}
			else 
			{
				jobs.front()->set_state(xeta::FileTransfer::Error);
			}
		}
		else 
		{
			jobs.front()->set_state(xeta::FileTransfer::Error);
		}
			

		log_fd = -1;
		wget_pid = -1;
		lock list_access( list_mutex );
		jobs.pop_front();
	}
}
*/
