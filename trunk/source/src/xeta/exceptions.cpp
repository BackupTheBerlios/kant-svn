#include "exceptions.h"
#include "../gettext.h"
#include <boost/format.hpp>
using namespace std;
using boost::format;

namespace xeta {

  RuntimeError::RuntimeError( const std::string &what ) throw(): runtime_error( what ) 
  { }

  PackageError::PackageError( const std::string &what ) throw(): RuntimeError( what )
  { }

  AmbiguousPackage::AmbiguousPackage( const string &canonical, const PackageList &packages ) throw(): 
    PackageError( (format(_("Package \"%1%\" is ambiguous"))%canonical).str() ), possible_packages(packages)
  { }

  AmbiguousPackage::~AmbiguousPackage() throw()
  { }

  const PackageList &AmbiguousPackage::get_possible_packages() const 
  { return possible_packages; }

  PackageNotFound::PackageNotFound( const string &canonical ) throw(): 
    PackageError( (format(_("Package \"%1%\" not found"))%canonical).str() ) 
  { } 

  SyntaxError::SyntaxError( const std::string &context, const std::string &message ) throw():
    RuntimeError( (format(_("Syntax error in %1%: \"%1%\"") )%context%message).str() )
  { }

  InitializationError::InitializationError( const std::string &what ) throw(): RuntimeError( what )
  { }

}
