#!/bin/bash

# (c) 2002 Bruno Rodrigues <bruno.rodrigues@litux.org>
# Under GPL Licence

# Add a new TRANSLATION line to every .pro file if there
# is already at least one TRANSLATION file and this LANG
# is not present
# The perl line would grab a TRANSLATION = something and
# duplicate it to TRANSLATION += .../LANG/...

LANG=$1

if [ "$1x" == "x" ] ; then
	echo "Usage: $0 <LANG>"
	exit
fi

for i in `find . -name "*.pro"` ; do 
	grep TRANSLATIONS $i > /dev/null 
	if [ "$?" != 0 ] ; then 
		echo "$i: No Translations" 
	else 
		grep "../i18n/$LANG/" $i > /dev/null 
		if [ "$?" == 0 ] ; then
			echo "$i: $LANG already there" 
		else 
	 		echo "$i: Adding $LANG" 
			perl -p -i.bak -e 's/^(TRANSLATIONS\s+)(([^+])?=)(.+?i18n\/)(.+?)(\/.+?)$/$1$2$4$5$6\n$1+=$4'$LANG'$6/' $i
		fi
	fi
done



