#!/bin/bash

PREFIX='http://purl.org/zynot/xeta/current/dtd/'

for opt
do
	if [ -n "${arginto}" ] && [ -n "${prevopt}" ] ; then
		"${arginto}" = "${opt}"
		arginto=
		prevopt=
		continue
	else
		case $opt in
			*=*) arg="${opt/#*=/}" ;;
			*) arg="" ;;
		esac
	fi
	
	case $opt in
		--prefix=* | -prefix=*)
			PREFIX="${arg}" ;;
		--prefix | -prefix | -p)
			arginto='PREFIX'
			prevopt='prefix'
			;;
		--help | -help | -h | -?)
			cat << EOF
Usage: make-dtdindex.sh [OPTIONS]...

Generates dtdindex.dtd from dtdindex.dtd.in

Defaults for the options are specified in brackets.

Files and directories:
  --prefix=PREFIX   Prefixes PREFIX to URIs.
                    [$PREFIX]
EOF
			exit 0 ;;
		*)
			echo "Unknown option $opt." 1>&2
			exit 1
			;;
	esac
done

if [ -n "${arginto}" ] && [ -n "${prevopt}" ] ; then
	echo "Option --${prevopt} needs an argument!" 1>&2
	exit 1
fi

if [ -f "dtdindex.dtd.in" ] ; then
	if [ -e "dtdindex.dtd" ] ; then
		echo "File dtdindex.dtd already exists!" 1>&2
		exit 1
	else
		EPREFIX="$(echo "${PREFIX}" | sed 's/\\/\\\\/g; s/:/\\:/g; s/&/\\&/g;')"
		echo '<!-- @doctype dtdindex PUBLIC "DTD Index" "'"${PREFIX}dtdindex.dtd"'" -->' > dtdindex.dtd 
		sed 's:^[[:space:]]*\([^[:space:]]\+\)[[:space:]]\+\([^[:space:]]\+\)[[:space:]]\+\("[^"]\+"\).*$:<!ENTITY % \1 PUBLIC \3 "'"${EPREFIX}"'\2">:g' < dtdindex.dtd.in >> dtdindex.dtd
	fi
else
	echo "File dtdindex.dtd.in missing!" 1>&2
	exit 1
fi
	


		
