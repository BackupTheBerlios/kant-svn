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

#ifndef XETA_INFOIMPLMYSQL_H_1598547802
#define XETA_INFOIMPLMYSQL_H_1598547802
#include "info.h"
#include <sqlite.h>

namespace xeta {

  class InfoImplSqlite: public InfoImpl {
    public:
      virtual std::string get_default_tree() const;
      virtual bool verify_package( const PackageSpec &package );
      virtual unsigned int get_latest_revision( const PackageSpec &package );
      virtual void list_packages( const PackageSpec &spec, std::vector<PackageSpec> &result );
      virtual bool verify_package_uuid( const std::string &uuid );
      virtual std::string get_dependencystring( const PackageSpec &spec );
      virtual void list_dependencies( const std::string &depstring, std::vector<PackageSpec> &result );
      virtual std::string get_ldesc( const std::string &uuid );
      virtual std::string get_sdesc( const std::string &uuid );
      InfoImplSqlite( const Config &config );
      ~InfoImplSqlite();
    private:
      sqlite *handle;
      char *sqlite_errors;
      std::string get_desc( const std::string &uuid, const std::string &table );
      static int fill_vector( void* param1, int cols, char** values, char** );
      typedef std::vector<std::string> inner_container;
      typedef std::vector<inner_container> outer_container;
  };
}

#endif

