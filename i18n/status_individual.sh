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

if [ "${arg}" = "todo" ]; then
	if [ "x$2"x = "xx" ]; then
		echo "missing second argument"
		exit
	fi
	for i in $2/*.ts; do echo $i - `grep translation "$i" | grep type=\"unfinished\" | wc -l` | grep -v ' 0' ; done
	exit
fi

for i in $1/*.ts; do echo $i - `grep translation "$i" | grep type=\"unfinished\" | wc -l` ; done

