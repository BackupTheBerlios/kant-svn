/***************************************************************************
 *   Copyright (C) 2004 by Andreas Pokorny                                 *
 *                                                                         *
 * This file is part of the Serializer.                                    *
 *                                                                         *
 * Serializer is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU Lesser General Public License as          *
 * published by the Free Software Foundation; either version 2 of the      *
 * License, or (at your option) any later version.                         *
 *                                                                         *
 * Serializer is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this software; if not, write to the Free Software            *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               *
 ***************************************************************************/

#ifndef DEBUG_HEADER_HELPER_VIM_D
# define DEBUG_HEADER_HELPER_VIM_D

// Can be used in vim to debug program flow:
// :!./your_binary  2>&1| tee errors.err
// :cf
// -> like :make and :grep 
# ifdef DEBUG
#    include <iostream>
#    include <string>
#  ifdef XETA_PRINT_FUNCTION
#    include <boost/current_function.hpp>
#    define DEBUG_COUT(A) std::cout << __FILE__ << ":" << __LINE__ << ":" << BOOST_CURRENT_FUNCTION << A
#  else 
#    define DEBUG_COUT(A) std::cout << __FILE__ << ":" << __LINE__ << ":" << A
#  endif
# else
#  define DEBUG_COUT(A) 
# endif

#endif
