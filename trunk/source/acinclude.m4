##
## $Header: /cvsroot/libmage/libMAGE/m4/ac_choose_boost.m4,v 1.9 2004/12/14 01:52:13 zaufi Exp $
##
##
## Copyright (c) 2003, 2004 by Zaufi
##
## AC_CHOOSE_BOOST([REQUIRED_VERSION], [REQUIRED_LIBS], [INCLUDE_PATH_HINT], [LIB_PATH_HINT], [ACTION-IF-FOUND, [ACTION-IF-NOT-FOUND]]])
##
## Set the following variables:
##  BOOST_VERSION   - same as defined in boost/version.hpp
##  BOOST_CPPFLAGS  - `-I' preprocessor option
##  BOOST_LDFLAGS   - `-L` linker option
##  BOOST_LIB_VERSION   - same as in boost/version.hpp used for autolink.. could be used to link libs
##
AC_DEFUN([AC_CHOOSE_BOOST],[
    ac_choose_boost_result='no'

    am_inc_boost_dir_default=ifelse([$3], , "/usr/include /usr/local/include", $3)
    am_lib_boost_dir_default=ifelse([$4], , "/usr/lib /usr/local/lib", $4)

    BOOST_CPPFLAGS=''
    BOOST_VERSION=''
    BOOST_LIB_VERSION=''
    BOOST_LDFLAGS=''
    BOOST_SO_EXTENSION=''
    BOOST_TOOLSET=''


    AC_ARG_WITH(boost_includedir,
      AS_HELP_STRING([--with-boost-includedir=DIR],
        [Use Boost library headers installed at specified path(default=check)]),
      [am_inc_boost_dir=$withval],
      [am_inc_boost_dir=$am_inc_boost_dir_default])
    AC_ARG_WITH(boost_libdir,
      AS_HELP_STRING([--with-boost-libdir=DIR],
        [Use Boost libraries installed at specified path (default=check)]),
      [am_lib_boost_dir=$withval],
      [am_lib_boost_dir=$am_lib_boost_dir_default])

## Check headers
    AC_CACHE_CHECK([where boost headers installed], [ac_cv_boost_headers_installed_at],
        [
        ac_cv_boost_headers_installed_at='not found'
        if test -n "$am_inc_boost_dir"; then
        for dir in $am_inc_boost_dir; do
        if test -f $dir/boost/version.hpp; then
        BOOST_CPPFLAGS="-I$dir"
        ac_cv_boost_headers_installed_at=$dir
        break
        fi
        done
        fi
        ])
    
    if test -n "$2" ; then 
      ac_cv_boost_temp_lib_names=''

      for x in $2 ; do
        ac_cv_boost_temp_lib_names="$ac_cv_boost_temp_lib_names $x"
      done

      echo $ac_cv_boost_temp_lib_names

      AC_CACHE_CHECK([where boost libraries $2 are installed], [ac_cv_boost_libs_installed_at],
        [
        ac_cv_boost_libs_installed_at='not found'
        if test -n "$am_lib_boost_dir"; then
          for library in $ac_cv_boost_temp_lib_names; do 
            ac_cv_boost_that_lib_name='not found'
            for dir in $am_lib_boost_dir; do
              if ls $dir/libboost_$library* 2>&1 > /dev/null; then
                ac_cv_boost_that_lib_name='found'
                BOOST_LDFLAGS="-L`dirname $dir`"
                ac_cv_boost_libs_installed_at=$dir
                break
              fi
            done
            if test "$ac_cv_boost_that_lib_name" = 'not found'; then
              ac_cv_boost_libs_installed_at='not found'
              break
            fi
          done
        fi
        ])
    fi

## TODO: Fair check libraries needed

## Check version
    if test $ac_cv_boost_headers_installed_at != 'not found'; then
    AC_CACHE_CHECK([boost version], [ac_cv_boost_version],
        [
        ac_cv_boost_version=`cat $ac_cv_boost_headers_installed_at/boost/version.hpp \
        | grep  '^# *define  *BOOST_VERSION  *[0-9]\+$' \
        | sed 's,^# *define  *BOOST_VERSION  *\([0-9]\+\)$,\1,'`
        ])
am_required_boost_version=ifelse([$1], , $ac_cv_boost_version, $1)
## Try to compile simple program
  AC_CACHE_CHECK([boost version >= $am_required_boost_version], [ac_cv_boost_version_ok],
      [
      ac_cv_boost_version_ok='no'
      old_CPPFLAGS=$CPPFLAGS
      CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
      AC_LANG_PUSH(C++)
      AC_COMPILE_IFELSE(
        [
#include <boost/version.hpp>
#if BOOST_VERSION < $am_required_boost_version
#error Boost version less than required
#endif
        ], [ac_cv_boost_version_ok='yes'])
      AC_LANG_POP(C++)
      CPPFLAGS=$old_CPPFLAGS
      ])
  if test $ac_cv_boost_version_ok = 'yes'; then
  BOOST_VERSION=$ac_cv_boost_version
  ac_choose_boost_result='yes'
  fi
  fi

  if test "$ac_choose_boost_result" = 'yes' -a "$ac_cv_boost_libs_installed_at" != 'not found'; then
        BOOST_LIB_VERSION=`cat $ac_cv_boost_headers_installed_at/boost/version.hpp \
        | grep  '^# *define  *BOOST_LIB_VERSION  *"[0-9]\+_[0-9]\+"$' \
        | sed 's,^# *define  *BOOST_LIB_VERSION  *"\([0-9]\+_[0-9]\+\)"$,\1,'`
        BOOST_SO_EXTENSION=`cat $ac_cv_boost_headers_installed_at/boost/version.hpp \
                           | grep  '^# *define  *BOOST_VERSION  *[0-9]\+$' \ 
                           | sed 's,^# *define  *BOOST_VERSION  *\([0-9]\)0*\([1-9][0-9]*\)\([0-9][0-9]\)$,\1.\2.\3,' \ 
                           | sed 's,00,0,'`
                        
        if test -n "$2"; then
          ac_temp_boost_first_libname=''
          for x in $2; do 
            ac_temp_boost_first_libname=$x
          done
          ac_temp_boost_first_lib=''
          for x in `ls $ac_cv_boost_libs_installed_at/libboost_$ac_temp_boost_first_libname-*-*-*.so*`; do
            ac_temp_boost_first_lib=$x
          done
          BOOST_TOOLSET=`echo $ac_temp_boost_first_lib \
                        | sed -e "s:.*$ac_temp_boost_first_libname::" \
                        | sed -e "s:-\(.*\)-.*-.*:\1:"`
        fi
ifelse([$5], , :, [$5])
  else
ifelse([$6], , :, [$6])
  fi

  AC_SUBST(BOOST_CPPFLAGS)
  AC_SUBST(BOOST_LDFLAGS)
  AC_SUBST(BOOST_VERSION)
  AC_SUBST(BOOST_LIB_VERSION)
  AC_SUBST(BOOST_SO_EXTENSION)
  AC_SUBST(BOOST_TOOLSET)
  ])

AC_DEFUN([AC_DEFINE_DIR], [
      test "x$prefix" = xNONE && prefix="$ac_default_prefix"
      test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
      ac_define_dir=`eval echo [$]$2`
      ac_define_dir=`eval echo [$]ac_define_dir`
      $1="$ac_define_dir"
      AC_SUBST($1)
      ifelse($3, ,
        AC_DEFINE_UNQUOTED($1, "$ac_define_dir"),
        AC_DEFINE_UNQUOTED($1, "$ac_define_dir", $3))
      ])


