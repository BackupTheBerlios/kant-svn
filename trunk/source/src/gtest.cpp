#include "gtest.h"
#include "gxt/deptreeview.h"
#include "gxt/deptreepopup.h"
#include <gtkmm/window.h>
#include <gtkmm/main.h>

void gshow_deptree( const xeta::PackageSpec &spec ) {
  Gtk::Main m(0,0);
  Gtk::Window w;
  gxt::DepTreeView dtv(spec);
  gxt::DepTreePopup dtp(dtv);
  w.add(dtv);
  dtv.show();
  Gtk::Main::run(w);
}

