#!/bin/sh
#
# Main
#
arg=$1
if [ "x${arg}x" = "xx" ]; then
	echo "Usage: ./status_indiviual XX"
	echo "       with XX as a languagecode"
	exit
fi

for i in $1/*.ts; do echo $i - `grep translation "$i" | grep type=\"unfinished\" | wc -l` ; done

