/***************************************************************************
 *   Copyright (C) 2004 by Andreas Pokorny <apokorny@zynot.org>            *
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

#ifndef XETA_TYPES_H

#ifdef HAVE_CONFIG_H
#include "../../config.h"

typedef unsigned char __u8;
typedef signed char __s8;

#if (SIZEOF_INT == 8)
typedef int		__s64;
typedef unsigned int	__u64;
#elif (SIZEOF_LONG == 8)
typedef long		__s64;
typedef unsigned long	__u64;
#elif (SIZEOF_LONG_LONG == 8)
#if defined(__GNUC__)
typedef __signed__ long long 	__s64;
#else
typedef signed long long 	__s64;
#endif
typedef unsigned long long	__u64;
#endif

#if (SIZEOF_INT == 2)
typedef	int		__s16;
typedef	unsigned int	__u16;
#elif (SIZEOF_SHORT == 2)
typedef	short		__s16;
typedef	unsigned short	__u16;
#else
#error: undefined 16 bit type
#endif

#if (SIZEOF_INT == 4)
typedef	int		__s32;
typedef	unsigned int	__u32;
#elif (SIZEOF_LONG == 4)
typedef	long		__s32;
typedef	unsigned long	__u32;
#elif (SIZEOF_SHORT == 4)
typedef	short		__s32;
typedef	unsigned short	__u32;
#else
#error: undefined 32 bit type
#endif
#else
#ifdef HAVE_LINUX_TYPES_H
#include <linux/types.h>
#else
#error config.h and linux/types.h is missing.  
#endif

#endif

namespace xeta
{
	typedef __u8 uint8;
	typedef __u8 sint8;

	typedef __s16 sint16;
	typedef __u16 uint16;
	
	typedef __s32 sint32;
	typedef __u32 uint32;

	typedef __s64 sint64;
	typedef __u64 uint64;
}


#endif 
