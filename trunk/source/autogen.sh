#!/bin/sh
export WANT_AUTOMAKE=1.8
aclocal && 
libtoolize && 
autoheader  && 
automake --add-missing -c --gnu &&
autoconf &&
echo "Done"

