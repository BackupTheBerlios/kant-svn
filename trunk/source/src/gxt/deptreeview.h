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

#ifndef GXT_DEPTREE_H_1974733693
#define GXT_DEPTREE_H_1974733693
#include <gtkmm/treestore.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include <glibmm/refptr.h>
#include "../xeta/gendeptree.h"
#include "widget.h"

namespace gxt {

  class DepTreeView: public Gtk::TreeView, public xeta::DepTree, public gxt::Widget {
    public:
      DepTreeView();
      DepTreeView( const xeta::PackageSpec &spec );
      virtual ~DepTreeView() throw();
      const xeta::PackageSpec *get_selected() const;
    protected:
      typedef Gtk::TreeModel::iterator iterator;
      virtual void doreload();
      virtual void refill_model();
      virtual bool on_popup_menu();
      virtual bool on_button_press_event( GdkEventButton *event );
      void insert_tree( const Gtk::TreeNodeChildren &it, SpecTree::iterator b, SpecTree::iterator e );
      class PackageColumn: public Gtk::TreeModelColumnRecord {
        public:
          Gtk::TreeModelColumn<const xeta::PackageSpec*> spec;
          Gtk::TreeModelColumn<Glib::ustring> tree;
          Gtk::TreeModelColumn<Glib::ustring> package;
          Gtk::TreeModelColumn<Glib::ustring> sdesc;
          Gtk::TreeModelColumn<Glib::ustring> version;
          Gtk::TreeModelColumn<Glib::ustring> uuid;
          PackageColumn();
      };
      PackageColumn columns;
    private:
      Glib::RefPtr<Gtk::TreeStore> tree_store;
      iterator root;
      void ini();
      void apply_package_to_iterator( iterator &it, const xeta::PackageSpec &ps );
  };

}

#endif

