/***************************************************************************
 *   Copyright (C) 2004 by Daniel Albuschat <dalbuschat@zynot.org>         *
 *                                                                         *
 * This file is part of the Xeta package manager.                          *
 *                                                                         *
 * Xeta is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU Lesser General Public License as          *
 * published by the Free Software Foundation; either version 2 of the      *
 * License, or (at your option) any later version.                         *
 *                                                                         *
 * Xeta is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this software; if not, write to the Free Software            *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               *
 ***************************************************************************/

#ifndef DOUT_H_389790258
#define DOUT_H_389790258
#include <ostream>
#include <iostream>

namespace debug {

  extern class dstream {
    private: 
      class dstream_wrapper {
        public:
          dstream_wrapper( dstream &str, int level ): str(str), level(level) 
          { }
          template<typename T>
            dstream_wrapper &operator<<( const T &t ) {
              str.print(t, level);
              return *this;
            }
        private:
          dstream &str;
          int level;
      };
    public:
      dstream( std::ostream &stream  = std::cout, int level = 0 );
      template<typename T>
        void print( const T &t, int loglevel ) {
          if( loglevel <= level )
            stream << t;
        }
      dstream_wrapper operator()( int lvl ) 
      { return dstream_wrapper( *this, lvl ); }
      void set_level( int newlevel );
      int get_level() const;
    private:
      int level;
      std::ostream &stream;
  } dout;

}

#endif

