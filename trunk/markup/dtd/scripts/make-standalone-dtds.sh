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
Usage: make-standalone-dtds.sh 

Generates non-self-contained DTDs.
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

if [ -e 'standalone/standalone.dtd.in' ] ; then
	echo 'File standalone/standalone.dtd.in already exists!' 1>&2
	exit 1
fi

egrep '^[[:space:]]*<!--[[:space:]]*@doctype[[:space:]]+.*-->' dtdindex.dtd | head -1 |
	sed 's:^[[:space:]]*<!--[[:space:]]*@doctype[[:space:]]\+[^[:space:]]\+[[:space:]]\+\(.*[^[:space:]]\)[[:space:]]*-->.*$:<!ENTITY % dtdindex \1>:' > standalone/standalone.dtd.in
cat << EOF >> standalone/standalone.dtd.in
%dtdindex;

EOF

cat standalone/standalone.dtd.in.in >> standalone/standalone.dtd.in

for i in `sed 's:^[[:space:]]*[^[:space:]]\+[[:space:]]\+\([^[:space:]]\+\).*$:\1:g' dtdindex.dtd.in`
do
	scripts/do-reftypes.sh --replace-with=CDATA --file=$i >> standalone/standalone.dtd.in
done
echo "<!ENTITY % OBJREF 'CDATA'>" >> standalone/standalone.dtd.in


if [ -e 'dtdindex.dtd' ] && [ -e 'dtdindex.dtd.in' ] ; then
	for i in *.dtd
	do
		if [ -f "$i" ] ; then
			cat standalone/standalone.dtd.in > standalone/$i
			echo >> standalone/$i
			egrep "/$i\"" dtdindex.dtd  | head -1 |
				sed 's:^[[:space:]]*<!ENTITY[[:space:]]\+%[[:space:]]\+\([^[:space:]]\+\).*$:%\1\;:' >> standalone/$i
		fi
	done
else
	echo "Needed files missing!" 1>&2
	exit 1
fi	


		
