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

#ifndef XETA_GENDEPTREE_H_1197284349
#define XETA_GENDEPTREE_H_1197284349
#include "packagespec.h"
#include "tree.h"
#include <ostream>

namespace xeta {

  class DepTree {
    public:
      typedef Tree<PackageSpec> SpecTree;
      DepTree( const PackageSpec &spec );
      DepTree();
      const SpecTree &get_tree() const;
      void reload();
      virtual ~DepTree() throw();
      void set_spec( const PackageSpec &spec );
      const PackageSpec &get_spec() const;
    protected:
      virtual void doreload();
      const PackageSpec *spec;
      SpecTree tree;
    private:
      void add_dependency( const PackageSpec &spec, SpecTree::iterator where );
  };

}

std::ostream &operator<<( std::ostream &lhs, const xeta::DepTree &rhs );

#endif

