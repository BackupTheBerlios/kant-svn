/***************************************************************************
 *   Copyright (C) 2004 by Daniel Albuschat <dalbuschat@zynot.org>         *
 *                                                                         *
 * This file is part of the GUI Xeta toolkit.                              *
 *                                                                         *
 * Gxt is free software; you can redistribute it and/or modify             *
 * it under the terms of the GNU Lesser General Public License as          *
 * published by the Free Software Foundation; either version 2 of the      *
 * License, or (at your option) any later version.                         *
 *                                                                         *
 * Gxt is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this software; if not, write to the Free Software            *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               *
 ***************************************************************************/

#ifndef GXT_DEPTREEPOPUP_H_1460153480
#define GXT_DEPTREEPOPUP_H_1460153480
#include "packagespecbin.h"
#include "deptreeview.h"
#include "menu.h"
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/menuitem.h>

namespace gxt {

  class DepTreePopup: public gxt::Menu {
    public:
      DepTreePopup( DepTreeView &tree );
      ~DepTreePopup();
    protected:
      virtual void on_show_package();
    private:
      DepTreeView &tree;
      Gtk::Window win;
      gxt::PackageSpecBin psb;
  };

}

#endif

