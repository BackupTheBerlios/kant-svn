#include "infoimplsqlite.h"
#include "exceptions.h"
#include "../gettext.h"
#include <boost/format.hpp>
#include <iostream>
#include <sstream>
using namespace std;
using boost::format;

namespace xeta {

  namespace {
    extern "C" int count_sqlite_rows( void *param1, int, char **, char ** ) {
      (*static_cast<int*>(param1))++;
      return 0;
    }
    extern "C" int get_first_as_string( void *param1, int cols, char **values, char ** ) {
      if( cols >= 1 )
        *static_cast<string*>(param1) = values[0];
      return 1;
    }
    extern "C" int get_first_as_int( void *param1, int cols, char **values, char ** ) {
      if( cols >= 1 ) {
        int integer = 0;
        stringstream str(values[0]);
        str >> integer;
        *static_cast<int*>(param1) = integer;
      }
      return 1;
    }
  }

  InfoImplSqlite::InfoImplSqlite( const Config &config ) {
    string db = config["sqlite_database"];
    if( !(handle = sqlite_open( db.c_str(), 0, 0 )) )
      throw InitializationError( (format(_("Could not load database \"%1%\" in sqlite module"))%db).str() );
  }

  InfoImplSqlite::~InfoImplSqlite() {
    sqlite_close(handle);
  }

  string InfoImplSqlite::get_default_tree() const {
    return "tree"; // No trees atm!
  }

  bool InfoImplSqlite::verify_package( const PackageSpec &package ) { 
    PackageSpec ps = package;
    return !ps.fetch_uuid().empty();
  }

  unsigned int InfoImplSqlite::get_latest_revision( const PackageSpec &package ) {
    stringstream sql;
    sql << "select x.revision from proj p left join versions v on "
      << "v.id_proj=p.id left join xbuilds x on x.id=v.id_xbuild where x.uuid=\""
      << package.get_uuid()
      << "\" order by x.revision desc limit 1";
    int integer = 0;
    int errorcode = sqlite_exec(
        handle,
        sql.str().c_str(),
        &get_first_as_int,
        &integer,
        &sqlite_errors );
    if( (errorcode != SQLITE_OK) && (errorcode != SQLITE_ABORT) ) {
      string error(sqlite_errors);
      sqlite_freemem(sqlite_errors);
      throw RuntimeError( (format(_("Error while executing \"%1%\", error message: %2%"))%sql.str()%error).str() );
    }
    sqlite_freemem(sqlite_errors);
    return integer;
  }

  void InfoImplSqlite::list_packages( const PackageSpec &spec, vector<PackageSpec> &result ) {
    vector<PackageSpec> data;
    string sql = "select x.uuid, x.revision, p.names, v.key_ from proj p left join versions v on v.id_proj=p.id left join xbuilds x on x.id=v.id_xbuild";
    if( !spec.get_uuid().empty() )
      sql += " where x.uuid=" + spec.get_uuid();
    else if( spec.get_package().empty() )
      sql += ""; // list all packages
    else if( spec.get_version().empty() )
      sql += " where p.names=\""+spec.get_package()+'\"';
    else
      sql += " where p.names=\""+spec.get_package()+"\" and v.key_=\""+spec.get_version()+'\"';
    sql += " order by x.revision desc";
    int errorcode = sqlite_exec( 
        handle,
          sql.c_str(),
          &fill_vector,
          &data,
          &sqlite_errors );
    if( errorcode != SQLITE_OK ) {
      string error(sqlite_errors);
      sqlite_freemem(sqlite_errors);
      throw RuntimeError( (format(_("Error while executing \"%1%\", error message: %2%"))%sql%error).str() );
    }
    sqlite_freemem(sqlite_errors);
    for( vector<PackageSpec>::const_iterator i = data.begin(); i != data.end(); i++ )
      result.push_back(*i);
  }

  bool InfoImplSqlite::verify_package_uuid( const string &uuid ) {
    int counter = 0;
    const string sql = (string("select id from xbuilds where uuid=") + uuid).c_str();
    int errorcode = sqlite_exec( 
        handle,
        sql.c_str(),
        &count_sqlite_rows,
        &counter,
        &sqlite_errors );
    if( errorcode != SQLITE_OK ) {
      string error(sqlite_errors);
      sqlite_freemem(sqlite_errors);
      throw RuntimeError( (format(_("Error while executing \"%1%\", error message: %2%"))%sql%error).str() );
    }
    sqlite_freemem(sqlite_errors);
    return counter == 1;
  }

  int InfoImplSqlite::fill_vector( void* param1, int cols, char** values, char** ) {
    inner_container tmpv(cols);
    for( int i = 0; i < cols; i++ ) {
      if( values[i] )
        tmpv[i] = values[i];
    }
    unsigned int revision = 0;
    stringstream str(tmpv[1]);
    str >> revision;
    if( !str ) 
      cerr << _("Invalid revision\n");
    static_cast<vector<PackageSpec>*>(param1)->push_back(
        PackageSpec("tree",tmpv[2],tmpv[3],revision,tmpv[0],false)
        );
    return 0;
  }

  string InfoImplSqlite::get_dependencystring( const PackageSpec &spec ) {
    string result;
    stringstream sql;
    sql << "select m.dep from xbuilds x left join metadata m on m.id_xbuild=x.id where x.uuid=\""
      << spec.get_uuid() << '\"';
    int errorcode = sqlite_exec(
        handle,
        sql.str().c_str(),
        &get_first_as_string,
        &result,
        &sqlite_errors );
    if( (errorcode != SQLITE_OK) && (errorcode != SQLITE_ABORT) ) {
      string error(sqlite_errors);
      sqlite_freemem(sqlite_errors);
      throw RuntimeError( (format(_("Error while executing \"%1%\", error message: %2%"))%sql%error).str() );
    }
    sqlite_freemem(sqlite_errors);
    return result;
  }

  void InfoImplSqlite::list_dependencies( const string &depstring, vector<PackageSpec> &result ) {
    stringstream str(depstring);
    string line;
    while( str >> line )
      result.push_back(PackageSpec(line,false,true));
  }

  string InfoImplSqlite::get_ldesc( const string &uuid )
  { return get_desc( uuid, "ldesc" ); }

  string InfoImplSqlite::get_sdesc( const string &uuid )
  { return get_desc( uuid, "sdesc" ); }

  string InfoImplSqlite::get_desc( const string &uuid, const string &table ) { 
    string result;
    stringstream sql;
    sql << "select d.text_ from " << table 
      << " d left join proj p on p.id=d.id_proj"
      << " left join versions v on v.id_proj=p.id"
      << " left join xbuilds x on x.id=v.id_xbuild"
      << " where d.lang=\"en\" and x.uuid=\"" 
      << uuid << '\"';
    int errorcode = sqlite_exec(
        handle,
        sql.str().c_str(),
        &get_first_as_string,
        &result,
        &sqlite_errors );
    if( (errorcode != SQLITE_OK) && (errorcode != SQLITE_ABORT) ) {
      string error(sqlite_errors);
      sqlite_freemem(sqlite_errors);
      throw RuntimeError( (format(_("Error while executing \"%1%\", error message: %2%"))%sql.str()%error).str() );
    }
    sqlite_freemem(sqlite_errors);
    return result;
  }

}

