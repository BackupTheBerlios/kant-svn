#include "packagespec.h"
#include "exceptions.h"
#include "info.h"
#include "../gettext.h"
#include <boost/format.hpp>
#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
using namespace std;
using boost::format;

namespace xeta {

  VersionSpec::VersionSpec()
    : op(NoOp), alpha(0), beta(0), release_candidate(0), revision(0), block(false)
  {
  }

  void VersionSpec::add_number( unsigned int num )
  {
    numbers.push_back( num );
  }

  void VersionSpec::set_operator( Operation operation )	{
		op = operation;
	}

  void VersionSpec::set_alpha( unsigned int num )
  {
    alpha =  num ;
  }
  
  void VersionSpec::set_beta( unsigned int num )
  {
    beta =  num ;
  }
  
  void VersionSpec::set_release_candidate( unsigned int num )
  {
    release_candidate =  num ;
  }
  
  void VersionSpec::set_revision( unsigned int num )
  {
    revision =  num ;
  }

  void VersionSpec::set_block( bool blocking )
  {
		block = blocking;
  }

	bool VersionSpec::get_block(  ) const  {
		return block;
  }

	unsigned int VersionSpec::get_alpha(  ) const  {
		return alpha;
  }
	
	unsigned int VersionSpec::get_beta(  ) const  {
		return beta;
  }
	
	unsigned int VersionSpec::get_release_candidate(  ) const  {
		return release_candidate;
  }
	
	unsigned int VersionSpec::get_revision(  ) const  {
		return revision;
  }

	void VersionSpec::print( std::ostream &out ) const
	{
		char *convert[] = {"no_op","==", ">", "<", "<=", ">=" };

		if( block ) out << '!';
		out << convert[op];
		std::ostream_iterator<unsigned int> ui_out( out, "." );

		std::copy( numbers.begin(), numbers.end(), ui_out );

		if( beta ) out << "_b" << beta;
		if( alpha ) out << "_a" << alpha ;
		if( release_candidate ) out << "_rc" << release_candidate;
		if( revision ) out << "_r" << revision;
	}

  PackageInfo::PackageInfo( std::string const & name, std::string const & tree )
    : internal_name( name ), tree_name( tree )
  {
  }

  void PackageInfo::add_flag(  std::string const& flag, bool status )
  {
    flags.push_back( std::pair<bool,std::string>(status, flag) );
  }
  
  void PackageInfo::set_version( VersionSpec const& version )
  {
    this->version = version;
  }
  
  void PackageInfo::set_name( std::string const& name )
  {
    internal_name = name;
  }
  
  void PackageInfo::set_tree( std::string const& name )
  {
    tree_name = name;
  }
  
  std::string const& PackageInfo::get_name( ) const
  {
    return internal_name;
  }
  
  std::string const& PackageInfo::get_tree( ) const
  {
    return tree_name;
  }
  
  VersionSpec const& PackageInfo::get_version( ) const
  {
    return version;
  }

	void PackageInfo::print( std::ostream &out ) const
	{
		out <<  "name: " << internal_name << "  tree: " << tree_name << "  version constraint:" << "  flags:";
		version.print( out );

		for( std::list<std::pair<bool, std::string> >::const_iterator it = flags.begin(), e = flags.end();
				it != e; ++it )
			out << "-+"[it->first] << it->second << " ";
		
	}

  PackageSpec::PackageSpec( const string &canonical, bool strict, bool uselatest ): revision(0) {
    string::size_type p;
    string spec = canonical;
    if( (p = spec.find( "::" )) != string::npos ) {
      tree = spec.substr(0,p);
      spec = spec.substr(p+2);
    } else 
      tree = default_tree();
    if( (p = spec.rfind('-')) != string::npos ) {
      package = spec.substr(0,p);
      spec = spec.substr(p+1);
      version = spec;
    } else
      if( spec.empty() )
        throw PackageError( (format(_("No package specified in \"%1%\""))%canonical).str() );
      else
        package = spec;
    if( strict || uselatest )
      uuid = find_uuid(uselatest); // will throw an exception, if package spec is not valid.
    revision = Info::instance().get_latest_revision( *this );
  }

  PackageSpec::PackageSpec( 
      const string &tree,
      const string &package,
      const std::string &version,
      unsigned int revision, 
      const string &puuid,
      bool strict ):
        tree(tree), package(package), version(version), uuid(puuid), revision(revision) {
          if( strict )
            uuid = find_uuid();
  }

  const string &PackageSpec::get_tree() const 
  { return tree; }

  const string &PackageSpec::get_package() const 
  { return package; }

  unsigned int PackageSpec::get_revision() const 
  { return revision; }

  const string &PackageSpec::get_uuid() const 
  { return uuid; }

  const string &PackageSpec::fetch_uuid() { 
    if( uuid.empty() )
      uuid = find_uuid();
    return uuid; 
  }

  const string &PackageSpec::get_version() const
  { return version; }

  string PackageSpec::default_tree() 
  { return "tree"; }

  bool PackageSpec::verify() { 
    bool result = Info::instance().verify_package(*this);
    if( result ) {
      fetch_ldesc();
      fetch_sdesc();
    }
    return result;
  }

  bool PackageSpec::verify( string &errormessage ) {
    try {
      return verify();
    } catch( PackageError &e ) {
      errormessage = e.what();
      return false;
    }
  }

  string PackageSpec::get_canonical() const {
    string canonical = tree+"::";
    canonical += package;
    if( !package.empty() )
      canonical += '-' + version;
    return canonical;
  }

  string PackageSpec::find_uuid( bool uselatest ) {
    const string &canonical = get_canonical();
    vector<PackageSpec> v;
    Info::instance().list_packages( *this, v );
    if( v.empty() )
      throw PackageNotFound( canonical );
    if( v.size() != 1 && !uselatest )
      throw AmbiguousPackage( canonical, v );
    return v[0].get_uuid();
  }

  const string &PackageSpec::fetch_ldesc() {
    if( ldesc.empty() )
      ldesc = Info::instance().get_ldesc(fetch_uuid());
    return ldesc;
  }

  const string &PackageSpec::fetch_sdesc() {
    if( sdesc.empty() )
      sdesc = Info::instance().get_sdesc(fetch_uuid());
    return sdesc;
  }

  const string &PackageSpec::get_ldesc() const 
  { return ldesc; }

  const string &PackageSpec::get_sdesc() const
  { return sdesc; }

}

std::ostream &operator << ( std::ostream &lhs, const xeta::PackageSpec &rhs ) {
  lhs << format(_("tree     : %1%"))%rhs.get_tree()     << '\n'
      << format(_("package  : %1%"))%rhs.get_package()  << '\n'
      << format(_("version  : %1%"))%rhs.get_version()  << '\n'
      << format(_("revision : %1%"))%rhs.get_revision() << '\n'
      << format(_("uuid     : %1%"))%rhs.get_uuid()     << '\n'
      << format(_("sdesc    : %1%"))%rhs.get_sdesc()    << '\n'
      << format(_("ldesc    : %1%"))%rhs.get_ldesc();
  return lhs;
}

std::ostream& operator<<( std::ostream& out, const xeta::PackageInfo & obj ) {
	obj.print( out );
	return out;
}

std::ostream& operator<<( std::ostream& out, const xeta::VersionSpec & obj ){
	obj.print( out );
	return out;
}
