#!/bin/bash

REPLACEMENT=CDATA
FILE=

for opt
do
	if [ -n "${arginto}" ] && [ -n "${prevopt}" ] ; then
		set "${arginto}"="${opt}"
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
		--replace-with=* | -replace-with=*)
			REPLACEMENT="${arg}" ;;
		--replace-with | -replace-with | -r)
			arginto=REPLACEMENT
			prevopt=replace-with
			;;
		--file=* | -file=*)
			FILE="${arg}" ;;
		--file | -file | -f)
			arginto=FILE
			prevopt=file
			;;
		--help | -help | -h | -?)
			cat << EOF
Usage: do-reftypes --replace-with=[REPLACEMENT] --file=[FILE]

Generates entities for @reftype commands in FILE.
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


REFTYPES="$(egrep '<!--[[:space:]]*@reftype([[:space:]]+[[:alpha:]_][[:alnum:]_-.]*)+[[:space:]]*-->' "${FILE}" |
			sed 's:<!--[[:space:]]*@reftype[[:space:]]\+::g; 
			     s:[[:space:]]*-->::g;
			     s:[[:space:]]\+: :g')"
			     
for i in ${REFTYPES}
do
	echo "<!ENTITY % $i '$REPLACEMENT'>"
done


		
