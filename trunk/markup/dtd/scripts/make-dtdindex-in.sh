#!/bin/bash

PREFIX=
SUFFIX='DTD'

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
		--suffix=* | -suffix=*)
			SUFFIX="${arg}" ;;
		--suffix | -suffix | -s)
			arginto='SUFFIX'
			prevopt='suffix'
			;;
		--help | -help | -h | -?)
			cat << EOF
Usage: make-dtdindex-in.sh [OPTIONS]...

Generates dtdindex.dtd.in.

Defaults for the options are specified in brackets.

Files and directories:
  --prefix=PREFIX   Prefixes PREFIX to entity names. [${PREFIX}]
  --suffix=SUFFIX   Suffixes SUFFIX to entity names. [${SUFFIX}]
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

gethash() {
	echo -n `echo -n "$*" | md5sum | sed 's:^\([^[:space:]]*\).*$:\1:g'`
}

dofile() {	
	f="${DTD_FILENAME[$1]}"
	ENTITYNAME="${PREFIX}$(basename "$f" .dtd)${SUFFIX}"
	ENTITYNAME="$(echo "${ENTITYNAME}" | sed 's:^[^[:alpha:]_]:A&:g; s:[^[[:alnum:]._-]:_:g')"
	if [ "${DTD_DEP_RESOLVED[$1]}" -eq 2 ] ; then
		echo 'Dependency cycle!' 1>&2
		exit 1
	fi
	
	if [ "${DTD_DEP_RESOLVED[$1]}" -eq 0 ] ; then
		LIST="$((for i in ${DTD_AFTER[$n]} ${DTD_DEPEND[$n]}; do echo $i; done) | uniq)"
		
		DTD_DEP_RESOLVED[$1]=2
		
		for i in ${DTD_BEFORE[$n]}
		do
			HASH="$(gethash $i)"
			j="MAP_${HASH}"
			j=${!j}
			if [ -z "$j" ] ; then
				echo "Missing dependency: $i from $f" 1>&2
				exit 1
			fi
			if [ "${DTD_DEP_RESOLVED[$n]}" -neq 0 ] ; then
				echo "Warning: $f is after $i, but was specified to be before $i" 1>&2
			fi
		done
		
		for i in ${LIST}
		do
			HASH="$(gethash $i)"
			j="MAP_${HASH}"
			j="${!j}"
			if [ -z "$j" ] ; then
				echo "Missing depedency: $i from $f" 1>&2
				exit 1
			fi

			dofile $j
		done		
	fi
	
	DTD_DEP_RESOLVED[$1]=1
	
	echo "$ENTITYNAME $f \"-//Zynot//DTD $(basename $f .dtd)//EN\"" >> dtdindex.dtd.in
}


if [ -e 'dtdindex.dtd.in' ] ; then
	echo "File dtdindex.dtd.in already exists!" 1>&2
	exit 1
else
	if  [ -f 'dtdindex.dtd.in.in' ] ; then
		cat dtdindex.dtd.in.in > dtdindex.dtd.in
	fi
	n=0
	for f in *.dtd
	do
		if [ -f "$f" ] ; then
			DTD_FILENAME[$n]="$f"
			DTD_DEPEND[$n]="$(egrep '<!--[[:space:]]*@depend([[:space:]]+[^[:space:]]+)+[[:space:]]*-->' $f | 
				sed 's:^.*<!--[[:space:]]*@depend[[:space:]]*::g; s:[[:space:]]*-->.*$::g; s:[[:space:]]\+: :g')"
			DTD_BEFORE[$n]="$(egrep '<!--[[:space:]]*@before([[:space:]]+[^[:space:]]+)+[[:space:]]*-->' $f | 
				sed 's:^.*<!--[[:space:]]*@before[[:space:]]*::g; s:[[:space:]]*-->.*$::g; s:[[:space:]]\+: :g')"
			DTD_AFTER[$n]="$(egrep '<!--[[:space:]]*@after([[:space:]]+[^[:space:]]+)+[[:space:]]*-->' $f | 
				sed 's:^.*<!--[[:space:]]*@after[[:space:]]*::g; s:[[:space:]]*-->.*$::g; s:[[:space:]]\+: :g')"
			DTD_USE[$n]="$(egrep  '<!--[[:space:]]*@use([[:space:]]+[^[:space:]]+)+[[:space:]]*-->' $f | 
				sed 's:^.*<!--[[:space:]]*@use[[:space:]]*::g; s:[[:space:]]*-->.*$::g; s:[[:space:]]\+: :g')"
			DTD_DEP_RESOLVED[$n]=0

			HASH="$(gethash $f)"
			export "MAP_${HASH}"="${n}"
			DTD_HASH[$n]="${HASH}"
			
			n=$(($n + 1))
		fi
	done
	
	export DTD_FILENAME DTD_DEPEND DTD_BEFORE DTD_AFTER DTD_USE DTD_DEP_RESOLVED DTD_HASH
		
	for ((i=0; i < n; i=i+1))
	do
		dofile $i
	done
fi


		
