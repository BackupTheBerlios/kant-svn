#include "info.h"
#include "exceptions.h"
#include "infoimplsqlite.h"
using namespace std;

namespace xeta {

  Info::Info(): config("xeta.conf"), ownsimpl(true) { 
    /* 
     * read configuration files and act accordingly 
     * We'll always use sqlite for now
     */
    pimpl = new InfoImplSqlite( config );
  }

  Info::Info( InfoImpl &impl ): pimpl(&impl), ownsimpl(false)
  { }

  Info::~Info() {
    if( ownsimpl )
      delete pimpl;
  }

  Info &Info::instance() {
    static Info i;
    return i;
  }

  unsigned int Info::get_latest_revision( const PackageSpec &package ) {
    return pimpl->get_latest_revision( package );
  }

  bool Info::verify_package( const PackageSpec &package, bool throw_on_error ) {
    bool result = false;
    try {
       pimpl->verify_package( package );
       result = true;
    } catch( RuntimeError &e ) {
      result = false;
      if( throw_on_error )
        throw;
    }
    return result;
  }

  string Info::get_default_tree() const {
    return pimpl->get_default_tree();
  }

  void Info::list_packages( const PackageSpec &spec, std::vector<PackageSpec> &result ) {
    pimpl->list_packages( spec, result );
  }

/*  string Info::search_package_category( const std::string &package, const std::string &tree ) {
    return pimpl->search_package_category( package, tree );
  }*/

  InfoImpl *Info::get_impl() 
  { return pimpl; }

  InfoImpl::~InfoImpl() 
  { }

  string Info::get_dependencystring( const PackageSpec &spec ) {
    return pimpl->get_dependencystring( spec );
  }
  void Info::list_dependencies( const string &depstring, vector<PackageSpec> &result ) {
    pimpl->list_dependencies( depstring, result );
  }

  string Info::get_ldesc( const std::string &uuid )
  { return pimpl->get_ldesc( uuid ); }

  string Info::get_sdesc( const std::string &uuid )
  { return pimpl->get_sdesc( uuid ); }

}
