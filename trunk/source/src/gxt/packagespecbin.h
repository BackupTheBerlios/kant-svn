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

#ifndef GXT_PACKAGESPECBIN_H_310225485
#define GXT_PACKAGESPECBIN_H_310225485
#include <gtkmm/bin.h>
#include <gtkmm/frame.h>
#include <gtkmm/table.h>
#include <gtkmm/label.h>
#include <gtkmm/textview.h>
#include <gtkmm/textbuffer.h>
#include "../xeta/packagespec.h"

namespace gxt {

  class PackageSpecBin: public Gtk::Frame {
    public:
      PackageSpecBin();
      PackageSpecBin( const xeta::PackageSpec &spec );
      void set_spec( const xeta::PackageSpec &spec );
      const xeta::PackageSpec &get_spec() const;
    private:
      void ini();
      void update();
      const xeta::PackageSpec *spec;
      Gtk::Table table;
      Gtk::Label ltree, lpackage, lversion, luuid, lsdesc, lrevision;
      Gtk::Label dtree, dpackage, dversion, duuid, dsdesc, drevision;
      Gtk::TextView tldesc;
      Glib::RefPtr<Gtk::TextBuffer> tb;
  };

}

#endif

