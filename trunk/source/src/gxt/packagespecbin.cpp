#include "packagespecbin.h"
#include "../gettext.h"
#include <sstream>

namespace gxt {

  void PackageSpecBin::ini() {
    add(table);
      table.attach( dtree,      0, 1, 0, 1 );
      table.attach( dpackage,   0, 1, 1, 2 );
      table.attach( dversion,   0, 1, 2, 3 );
      table.attach( drevision,  0, 1, 4, 5 );
      table.attach( duuid,      0, 1, 6, 7 );
      table.attach( dsdesc,     0, 1, 7, 8 );
                                           
      table.attach( ltree,      1, 2, 0, 1 );
      table.attach( lpackage,   1, 2, 1, 2 );
      table.attach( lversion,   1, 2, 2, 3 );
      table.attach( lrevision,  1, 2, 4, 5 );
      table.attach( luuid,      1, 2, 6, 7 );
      table.attach( lsdesc,     1, 2, 7, 8 );

      table.attach( tldesc,     0, 2, 9, 10 );

    dtree.set_text     (  _("Tree: ")              );
    dpackage.set_text  (  _("Package: ")           );
    dversion.set_text  (  _("Version: ")           );
    duuid.set_text     (  _("UUID: ")              );
    dsdesc.set_text    (  _("Short description: ") );
    drevision.set_text (  _("Revision: ")          );
    tb = Gtk::TextBuffer::create();
    tldesc.set_buffer(tb);

    show_all_children();
    update();
  }

  PackageSpecBin::PackageSpecBin(): spec(0), table(10,2)
  { ini(); }

  PackageSpecBin::PackageSpecBin( const xeta::PackageSpec &spec ): spec(&spec), table(10,2)
  { ini(); }

  void PackageSpecBin::set_spec( const xeta::PackageSpec &spec_ ) { 
    spec = &spec_;
    update(); 
  }

  const xeta::PackageSpec &PackageSpecBin::get_spec() const 
  { return *spec; }

  void PackageSpecBin::update() {
    if( spec == 0 ) {
      ltree.set_text    ( "" );
      lpackage.set_text ( "" );
      lversion.set_text ( "" );
      luuid.set_text    ( "" );
      lsdesc.set_text   ( "" );
      lrevision.set_text( "" );
      tb->set_text      ( "" );
    } else {
      ltree.set_text    ( spec->get_tree()    );
      lpackage.set_text ( spec->get_package() );
      lversion.set_text ( spec->get_version() );
      luuid.set_text    ( spec->get_uuid()    );
      lsdesc.set_text   ( spec->get_sdesc()   );
      std::stringstream str;
      str << spec->get_revision();
      lrevision.set_text( str.str()          );
      tb->set_text(       spec->get_ldesc()  );
    }
  }

}
