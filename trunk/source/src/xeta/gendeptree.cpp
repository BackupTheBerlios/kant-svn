#include "../gettext.h"
#include "gendeptree.h"
#include "exceptions.h"
#include "info.h"
#include <boost/format.hpp>
#include <vector>
#include <string>
#include <sstream>
using namespace std;
using boost::format;

namespace xeta {

  DepTree::DepTree( const PackageSpec &spec ): spec(&spec) 
  { reload(); }

  DepTree::DepTree(): spec(0)
  { }

  const DepTree::SpecTree &DepTree::get_tree() const
  { return tree; }

  void DepTree::reload() { 
    if( spec ) {
      tree.clear();
      doreload(); 
    }
  }

  void DepTree::doreload()
  { add_dependency( *spec, tree.begin() ); }

  void DepTree::add_dependency( const PackageSpec &spec, DepTree::SpecTree::iterator where ) {
    try {
      string depstr = Info::instance().get_dependencystring( spec );
      vector<PackageSpec> v;
      Info::instance().list_dependencies( depstr, v );
      for( vector<PackageSpec>::iterator i = v.begin();
          i != v.end(); ++i ) {
        i->verify();
        where = tree.insert( where, *i );
        add_dependency( *i, where.first_child() );
      } 
    } catch( PackageError &e ) {
      std::cerr << format(_("Error while building dependency graph: \"%1%\"\n"))%e.what();
    }
  }

  DepTree::~DepTree() throw() 
  { } 

  void DepTree::set_spec( const PackageSpec &spec_ ) { 
    spec = &spec_; 
    reload();
  }

  const PackageSpec &DepTree::get_spec() const
  { return *spec; }

}

std::ostream &operator<<( std::ostream &lhs, const xeta::DepTree &rhs ) { 
  print_tree( rhs.get_tree().begin(), rhs.get_tree().end(), "", lhs ); 
  return lhs;
}
