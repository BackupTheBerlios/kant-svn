/***************************************************************************
 *   Copyright (C) 2004 by Daniel Albuschat <dalbuschat@zynot.org>         *
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

#ifndef XETA_INFO_H_1901559711
#define XETA_INFO_H_1901559711
#include "forwarddecl.h"
#include "config.h"
#include <string>
#include <vector>

namespace xeta {

  class InfoImpl {
    public:
      virtual ~InfoImpl();
      virtual std::string get_default_tree() const = 0;
      virtual bool verify_package( const PackageSpec &package ) = 0;
      virtual unsigned int get_latest_revision( const PackageSpec &package ) = 0;
      virtual void list_packages( const PackageSpec &spec, std::vector<PackageSpec> &result ) = 0;
      virtual bool verify_package_uuid( const std::string &uuid ) = 0;
      virtual std::string get_dependencystring( const PackageSpec &spec ) = 0;
      virtual void list_dependencies( const std::string &depstring, std::vector<PackageSpec> &result ) = 0;
      virtual std::string get_ldesc( const std::string &uuid ) = 0;
      virtual std::string get_sdesc( const std::string &uuid ) = 0;
  };

  class Info {
    public:
      static Info &instance();
      unsigned int get_latest_revision( const PackageSpec &package );
      bool verify_package( const PackageSpec &package, bool throw_on_error = false );
      std::string get_default_tree() const;
      void list_packages( const PackageSpec &spec, std::vector<PackageSpec> &result );
      bool verify_package_uuid( const std::string &uuid );
      std::string get_dependencystring( const PackageSpec &spec );
      void list_dependencies( const std::string &depstring, std::vector<PackageSpec> &result );
      std::string get_ldesc( const std::string &uuid );
      std::string get_sdesc( const std::string &uuid );
      ~Info();
    protected:
      Info();
      Info( InfoImpl &impl );
      InfoImpl *get_impl();
    private:
      InfoImpl *pimpl;
      Config config;
      bool ownsimpl;
  };

}

#endif

