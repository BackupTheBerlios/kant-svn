#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(xeta, 0.0.1, http://bugs.zynot.org)
AM_INIT_AUTOMAKE(xeta, 0.0.1)
AC_CONFIG_SRCDIR([src/gtest.cpp])
AC_CONFIG_HEADER([config.h])
XETA_RELEASE=1.0
XETA_LIBRARY_VERSION=1:0:0
AC_SUBST(XETA_LIBRARY_VERSION)
AC_SUBST(XETA_RELEASE)

# AM_MAINTAINER_MODE

AC_LANG_CPLUSPLUS

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

AC_MSG_CHECKING(for NO debug flag)
AC_ARG_ENABLE(nodebug,
[  --enable-nodub     compile without debugging information],
        nodebug=$enableval,nodebug=no)
if test "$nodebug" != no
then 
	AC_DEFINE(DEBUG,1,use debugging info)
fi
AC_MSG_RESULT($enableval)

# Checks for libraries.
# TODO: Do this check depending on the chosen 
#       sql backend
PKG_CHECK_MODULES(SQLITE, 
				  sqlite,
				  ,
				  [AC_MSG_ERROR(libsqlite is missing.)] 
				  )

# gtkmm >=2.4 uses sigc++-2.0 
PKG_CHECK_MODULES(GTKMM,
                  gtkmm-2.0 sigc++-1.2,
                  ,
				 [AC_MSG_ERROR(gtkmm-2.0 is missing.)] )

AC_ARG_WITH(expat,
    [ --with-expat=<path> prefix of expat installation. e.g. /usr/local or /usr],
    [EXPAT_PREFIX=$with_expat],
    [EXPAT_PREFIX="/usr/"])


AC_SUBST(EXPAT_PREFIX)
AC_CHECK_LIB(expat, XML_ParserCreate, , [AC_MSG_ERROR("Could not find expat in ${EXPAT_PREFIX}.")] , ["-L${EXPAT_PREFIX}/lib/"] )

EXPAT_LIBS="-L${EXPAT_PREFIX}/lib/ -lexpat"
EXPAT_CFLAGS="-I${EXPAT_PREFIX}/include"
AC_SUBST(EXPAT_LIBS)
AC_SUBST(EXPAT_CFLAGS)

# Checking for libcurl
CURLPP_LIBS=""
CURLPP_CFLAGS=""
if test -e /usr/bin/curlpp-config
then
	CURLPP_CFLAGS=`curlpp-config --cflags`
	CURLPP_LIBS=`curlpp-config --libs`
	echo "CURLPP_LIBS is $CURLPP_LIBS"
	echo "CURLPP_CFLAGS is $CURLPP_CFLAGS"
else
	AC_MSG_WARN("Could not find curlpp-config, curl support disabled")
fi
AC_SUBST(CURLPP_CFLAGS)
AC_SUBST(CURLPP_LIBS)


# Typesizes used in xeta_types.h
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)
SIZEOF_SHORT=$ac_cv_sizeof_short
SIZEOF_INT=$ac_cv_sizeof_int
SIZEOF_LONG=$ac_cv_sizeof_long
SIZEOF_LONG_LONG=$ac_cv_sizeof_long_long
AC_DEFINE(SIZEOF_SHORT=$ac_cv_sizeof_short)
AC_SUBST(SIZEOF_SHORT)
AC_SUBST(SIZEOF_INT)
AC_SUBST(SIZEOF_LONG)
AC_SUBST(SIZEOF_LONG_LONG)
AC_C_BIGENDIAN

AC_CHECK_HEADERS(linux/types.h cstdlib unistd.h sys/sockio.h sys/sockio.h net/if.h netinet/in.h sys/ioctl.h)

# Checks for header files.
AC_CHECK_HEADERS(boost/call_traits.hpp boost/mpl/vector.hpp,
				 ,
				 [AC_MSG_ERROR("boost headers are missing.")])


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

AM_GNU_GETTEXT

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_OUTPUT(Makefile src/Makefile src/xeta/Makefile src/util/Makefile src/serialize/Makefile intl/Makefile po/Makefile.in m4/Makefile xeta-1.0.pc)
