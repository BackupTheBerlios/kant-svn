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



#ifndef XETA_PACKAGESPEC_H_121416013
#define XETA_PACKAGESPEC_H_121416013
#include <string>
#include <vector>
#include <list>
#include <cctype>
#include <ostream>

namespace xeta {

  class VersionSpec {
    public:
      enum Operation { 
        NoOp,
        Is,
        GreaterThan,
        LessThan,
        Greater,
        Less
      };
    private:
      Operation op;
      std::vector<unsigned int> numbers;
      unsigned int alpha, beta, release_candidate, revision;
			bool block; ///< if true this will inverse the state of the package
      std::string version_string; ///< version string without the compare operation
    public:
      VersionSpec();
      void add_number( unsigned int num );
      void set_operator( Operation op );
      void set_alpha( unsigned int num );
      void set_beta( unsigned int num );
      void set_release_candidate( unsigned int num );
      void set_revision( unsigned int num );
			void set_block( bool blocking = true );

			bool get_block( ) const;
			unsigned int get_alpha( ) const;
			unsigned int get_beta( ) const;
			unsigned int get_release_candidate( ) const;
			unsigned int get_revision( ) const;

			void print( std::ostream & out ) const;
  };

  class PackageInfo {
    private:
      VersionSpec version;
      std::string internal_name, tree_name;
      std::list<std::pair<bool, std::string> > flags;
    public:
      PackageInfo( std::string const & name = "", std::string const & tree = "" );
      void add_flag(  std::string const& flag, bool status = true );
      void set_version( VersionSpec const& version );
      void set_name( std::string const& name );
      void set_tree( std::string const& name );
      std::string const& get_name( ) const;
      std::string const& get_tree( ) const;
      VersionSpec const& get_version( ) const;

			void print( std::ostream & out ) const;
  };

 /** 
 *  A package specification is used to identify a
 *  package. You can either initialize it with a canonical representation,
 *  the tree, package and revision, or directly with an uuid. 
 *  The missing information will be loaded from the database backend.
 *  Optionally, you can use it in a strict mode. It'll check integrity on
 *  every change. If the package specification is not appropriate, (i.e.
 *  the package does not exist or is ambiguous) it'll throw an exception.
 *  This class may be used as a basic search-facility. But there will be a
 *  fast, flexible full text search in xeta.
 *  Note: The term `not valid' means, the package could either
 *        not be found or is ambigious. Both will create different
 *        error conditions.
 */ 
  class PackageSpec {
    private:
      std::string tree, package, version, uuid;
      std::string sdesc, ldesc;
      unsigned int revision;
      std::string default_tree();
      std::string find_uuid( bool uselatest = false );
    public:
      // Both ctors may throw exceptions if the package is not valid and the
      // strict mode is chosen.
      PackageSpec( const std::string &canonical, bool strict = false, bool uselatest = false );
      PackageSpec( 
          const std::string &tree,
          const std::string &package,
          const std::string &version,
          unsigned int revision = 0,
          const std::string &uuid = "", bool strict = false );
      // Will throw exception if the package is not valid
      bool verify();
      // Won't throw a xeta-exception, but put the error message in `errormessage'.
      bool verify( std::string &errormessage );
      const std::string &get_tree() const;
      const std::string &get_package() const;
      const std::string &get_version() const;
      const std::string &get_uuid() const;
      const std::string &get_ldesc() const;
      const std::string &get_sdesc() const;
      const std::string &fetch_uuid();
      const std::string &fetch_ldesc();
      const std::string &fetch_sdesc();
      unsigned int get_revision() const;
      std::string get_canonical() const;
  };

}

std::ostream &operator << ( std::ostream &lhs, const xeta::PackageSpec &rhs );
std::ostream& operator<<( std::ostream& out, const xeta::PackageInfo & obj );
std::ostream& operator<<( std::ostream& out, const xeta::VersionSpec & obj );

#endif

