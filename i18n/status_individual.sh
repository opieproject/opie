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
	for i in $2/*.ts; do 
		echo $i - `grep translation "$i" | grep type=\"unfinished\" | wc -l` | grep -v ' 0' ; 
	done
	exit
fi

if [ "${arg}" = "overview" ]; then
	if [ "x$2"x = "xx" ]; then
		echo "missing second argument"
		exit
	fi

	strs=`find $2 -path './unmaintained' -prune -o -path './en' -prune -o -path './xx' -prune -o -name "*.ts" -exec grep 'translation' {} \; | wc -l`
	unfi=`find $2 -path './unmaintained' -prune -o -path './en' -prune -o -path './xx' -prune -o -name "*.ts" -exec grep 'type=\"unfinished\"' {} \; | wc -l`
	obso=`find $2 -path './unmaintained' -prune -o -path './en' -prune -o -path './xx' -prune -o -name "*.ts" -exec grep 'type=\"obsolete\"' {} \; | wc -l`
	printf "\n%12s %5s\n" "Total:" ${strs}
	printf "%12s %5s %s\n" "Unfinished:" ${unfi} "[`expr \( ${strs} - ${unfi} \) \* 100 / ${strs}`% done]"
	printf "%12s %5s %s\n\n" "Obsolete:" ${obso} "[`expr ${obso} \* 100 / ${strs}`%]"
	exit
fi
	

for i in $1/*.ts; do echo $i - `grep translation "$i" | grep type=\"unfinished\" | wc -l` ; done

