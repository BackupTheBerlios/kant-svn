#!/bin/bash

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
		--help | -help | -h | -?)
			cat << EOF
Usage: make-monolith-dtd.sh 

Generates monolith.dtd from monolith.dtd.in
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

if [ -e 'monolith.dtd' ] ; then
	echo "File monolith.dtd already exists!" 1>&2
	exit 1
else
	if [ -e 'monolith.dtd.in' ] && [ -e 'dtdindex.dtd.in' ]; then
		cat monolith.dtd.in > monolith.dtd
		sed 's:^[[:space:]]*<!ENTITY[[:space:]]\+%[[:space:]]\+\([^[:space:]]\+\)[[:space:]]\+\(PUBLIC\|SYSTEM\)[[:space:]]\+\(.*[^[:space:]]\)[[:space:]]*>:<!ENTITY % M_\1 \2 \3>:g;
			s:<!--[[:space:]]*@[^[:space:]]\+.*-->::g' < dtdindex.dtd >> monolith.dtd
		echo >> monolith.dtd
		sed 's:^\([^[:space:]]\+\).*$:<!ENTITY % \1 '\'\ \''>:g' < dtdindex.dtd.in >> monolith.dtd
		echo >> monolith.dtd
		for i in `egrep '^[^[:space:]]+[[:space:]]+[^[:space:]]+' dtdindex.dtd.in | awk '{print $2}'`
		do
			scripts/do-reftypes.sh --replace-with=IDREF --file=$i >> monolith.dtd
		done
		echo "<!ENTITY % OBJREF 'IDREF'>" >> monolith.dtd
		echo >> monolith.dtd
		sed 's:^\([^[:space:]]\+\).*$:%M_\1\;:g' < dtdindex.dtd.in >> monolith.dtd
	else
		echo "Needed files missing!" 1>&2
		exit 1
	fi
fi	


		
