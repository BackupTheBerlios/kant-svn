#include "debug.h"
using namespace std;

namespace debug {

  dstream::dstream( ostream &stream, int level ): 
    level(level), stream(stream) 
  { }

  void dstream::set_level( int newlevel )
  { level = newlevel; }

  int dstream::get_level() const
  { return level; }

  dstream dout;

}
