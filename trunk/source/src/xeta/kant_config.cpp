#include "kant_config.h"
#include "exceptions.h"
#include "../gettext.h"
#include <boost/format.hpp>
#include <fstream>
#include <sstream>
using namespace std;
using boost::format;

namespace xeta {
  Config::Config( const string &filename, bool strict ) {
    ifstream file( filename.c_str() );
    string line;
    unsigned int lineno = 0;
    while( getline( file, line ) ) {
      lineno++;
      string::size_type p = line.find_first_not_of(" \t");
      if( p != string::npos && line[p] == '#' )
        continue;
      p = line.find( '=' );
      if( p == string::npos )
        if( strict ) {
          stringstream str;
          str << lineno;
          throw SyntaxError( filename, (format(_("Syntax error in line %1%"))%str.str()).str());
        }
        else
          continue;
      string key = line.substr(0,p);
      map[key] = line.substr(p+1);
    }
  }

  string Config::operator[]( const std::string &index ) const {
    if( map.find(index) != map.end() )
      return map[index];
    else
      return "";
  }
}

