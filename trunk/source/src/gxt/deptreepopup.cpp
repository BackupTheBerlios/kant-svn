#include "deptreepopup.h"
#include "../gettext.h"
#include <iostream>

namespace {
  extern "C" void detacher_donothing( GtkWidget *, GtkMenu * ) 
  { }
}

namespace gxt {

  DepTreePopup::DepTreePopup( DepTreeView &tree ): tree(tree) { 
    items().push_back(
        Gtk::Menu_Helpers::MenuElem(_("_Show"),
          SigC::slot(*this, &DepTreePopup::on_show_package) ) );
    tree.connect_menu(*this); 
    win.add(psb);
    psb.show();
  }

  DepTreePopup::~DepTreePopup() 
  { tree.disconnect_menu(*this); }

  void DepTreePopup::on_show_package() {
    const xeta::PackageSpec *sel = tree.get_selected();
    if( sel ) {
      psb.set_spec(*sel);
      win.show();
    }
  }

}
