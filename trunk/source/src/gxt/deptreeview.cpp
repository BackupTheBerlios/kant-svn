#include "deptreeview.h"
#include "../gettext.h"
#include <iostream>
using namespace Gtk;
using namespace xeta;

namespace gxt {

  void DepTreeView::ini() {
    tree_store = Gtk::TreeStore::create(columns);
    set_model(tree_store);
    append_column( _("Tree"), columns.tree );
    append_column( _("Package"), columns.package );
    append_column( _("Version"), columns.version );
    append_column( _("Description"), columns.sdesc );
    append_column( _("UUID"), columns.uuid );
  }

  DepTreeView::DepTreeView() 
  { ini(); }

  DepTreeView::DepTreeView( const PackageSpec &spec ): xeta::DepTree() { 
    ini(); 
    set_spec(spec);
  }

  DepTreeView::~DepTreeView() throw()
  { }

  void DepTreeView::doreload() {
    xeta::DepTree::doreload();
    refill_model();
  }

  void DepTreeView::refill_model() {
    tree_store->clear();
    root = tree_store->append();
    apply_package_to_iterator(root, *spec);
    insert_tree( root->children(), tree.begin(), tree.end() );
  }

  void DepTreeView::insert_tree( const Gtk::TreeNodeChildren &it, SpecTree::iterator b, SpecTree::iterator e ) {
    iterator i2;
    for( SpecTree::iterator i = b; i != e; ++i ) {
      i2 = tree_store->append( it );
      apply_package_to_iterator(i2, *i);
      insert_tree(i2->children(), i.first_child(), i.end_child() );
    }
  }

  DepTreeView::PackageColumn::PackageColumn() {
    add(spec);
    add(tree);
    add(package);
    add(sdesc);
    add(version);
    add(uuid);
  }

  void DepTreeView::apply_package_to_iterator( iterator &it, const xeta::PackageSpec &ps ) {
      (*it)[columns.tree]    = ps.get_tree();
      (*it)[columns.package] = ps.get_package();
      (*it)[columns.sdesc]   = ps.get_sdesc();
      (*it)[columns.version] = ps.get_version();
      (*it)[columns.uuid]    = ps.get_uuid();
      (*it)[columns.spec]    = &ps;
  }

  bool DepTreeView::on_popup_menu() {
    menu_popup();
    return true;
  }

  const xeta::PackageSpec *DepTreeView::get_selected() const {
    Gtk::TreeStore::iterator i = const_cast<DepTreeView*>(this)->get_selection()->get_selected();
    if( i )
      return (*i)[columns.spec];
    else
      return 0;
  }

  bool DepTreeView::on_button_press_event( GdkEventButton *event ) {
    Gtk::TreeView::on_button_press_event(event);
    if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ) {
      menu_popup();
      return true;
    }
    return false;
  }

}
