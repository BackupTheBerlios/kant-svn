#include "xeta/packagespec.h"
#include "xeta/exceptions.h"
#include "xeta/gendeptree.h"
#include "xeta/tree.h"
#include "gtest.h"
#include "gettext.h"
#include <boost/format.hpp>
#include <stdexcept>
#include <iostream>

int main( int argc, char *argv[] ) {
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);
  using namespace xeta;
  using namespace std;
  using boost::format;
  if( argc < 2 )
    return 1;
  try {
    PackageSpec ps( argv[1], true );
    ps.fetch_uuid();
    ps.fetch_ldesc();
    ps.fetch_sdesc();
    cout << ps << '\n';
    if( argc == 3 && string(argv[2]) == "-g" )
      gshow_deptree( ps );
    else {
      DepTree dt( ps );
      cout << dt << '\n';
    }
  } catch( AmbiguousPackage &p ) {
    cout << _("Package is ambiguous. Possible packages are: \n");
    const PackageList &l = p.get_possible_packages();
    for( PackageList::const_iterator i = l.begin(); i != l.end(); i++ )
      cout << i->get_canonical() << '\n';
  } catch( RuntimeError &e ) {
    cout << format(_("Exception caught: \"%1%\"\n")) % e.what();
  } catch( runtime_error &e ) {
    cout << format(_("Unknown exception caught: \"%1%\"\n")) % e.what();
  }
}

