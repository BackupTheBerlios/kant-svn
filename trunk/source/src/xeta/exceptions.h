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

#ifndef XETA_EXCEPTIONS_H_93284293
#define XETA_EXCEPTIONS_H_93284293
#include "packagespec.h"
#include <stdexcept>
#include <string>
#include <vector>

namespace xeta {

  typedef std::vector<PackageSpec> PackageList;

  class RuntimeError: public std::runtime_error {
    public:
      RuntimeError( const std::string &what ) throw();
  };

  class PackageError: public RuntimeError {
    public:
      PackageError( const std::string &what ) throw();
  };

  class AmbiguousPackage: public PackageError {
    public:
      AmbiguousPackage( const std::string &canonical, const PackageList &packages ) throw();
      virtual ~AmbiguousPackage() throw();
      const PackageList &get_possible_packages() const;
    private:
       PackageList possible_packages;
  };

  class PackageNotFound: public PackageError {
    public:
      PackageNotFound( const std::string &canonical ) throw();
  };

  class SyntaxError: public RuntimeError {
    public:
      SyntaxError( const std::string &context, const std::string &message ) throw();
  };

  class InitializationError: public RuntimeError {
    public:
      InitializationError( const std::string &what ) throw();
  };

}

#endif

