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

#ifndef GXT_WIDGET_H_1373947643
#define GXT_WIDGET_H_1373947643
#include <gtkmm/menu.h>
#include <gtkmm/widget.h>
#include "menu.h"

namespace gxt {

  class Widget {
    public:
      Widget();
      virtual ~Widget();
      void connect_menu( gxt::Menu &menu );
      void disconnect_menu();
      void disconnect_menu( gxt::Menu &menu );
    protected:
      Gtk::Menu *menu;
      void menu_popup();
      void menu_popup( guint button, guint32 activate_time );
  };

}

#endif

