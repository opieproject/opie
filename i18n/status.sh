#!/bin/sh
lang=`basename ${1} 2>/dev/null | tr -d '/'`
[ "x${2}x" = "xunfinishedx" ] && unfinishedonly=y

if [ "x${lang}x" = "xx" ]; then
	echo "Usage: `basename ${0}` <XX> [unfinished]"
	echo "       with <XX> as a languagecode or \"all\" for all languagecodes."
	echo "       If you specify 'unfinished' after the languagecode,"
	echo "       only ts-files with unfinished strings will be shown."
	exit 1
else
	case "${lang}" in
	"all")
		clear
		printf "\ni18n Statistics for ALL languagecodes\n"
		lang="."
		;;
	"unmaintained"|"xx"|"en")
		echo "Specified languagecode not allowed."
		exit 1
		;;
	*)
		if [ ! -d "${lang}" ]; then
			echo "Specified languagecode not available."
			exit 1
		fi
		clear
		printf "\ni18n statistics for languagecode \"${lang}\"\n\n"
		echo " .--------- Total strings"
		echo " |   .----- Unfinished strings"
		echo " |   |   .- Obsolete strings"
		echo " |   |   |  File Name"
		echo "--- --- --- -------------------------------------"
		for file in `ls -1 ${1}/*.ts`; do
			strs=`grep -c 'translation' ${file}`
			unfi=`grep -c 'type=\"unfinished\"' ${file}`
			obso=`grep -c 'type=\"obsolete\"' ${file}`
			if [ ${unfi} -gt 0 -o "x${unfinishedonly}x" = "xx" ]; then
				printf "%3s %3s %3s %s\n" "${strs}" "${unfi}" "${obso}" "${file}"
			fi
		done
		;;
	esac
	strs=`find ${lang} -path './unmaintained' -prune -o -path './en' -prune -o -path './xx' -prune -o -name "*.ts" -exec grep 'translation' {} \; | wc -l`
	unfi=`find ${lang} -path './unmaintained' -prune -o -path './en' -prune -o -path './xx' -prune -o -name "*.ts" -exec grep 'type=\"unfinished\"' {} \; | wc -l`
	obso=`find ${lang} -path './unmaintained' -prune -o -path './en' -prune -o -path './xx' -prune -o -name "*.ts" -exec grep 'type=\"obsolete\"' {} \; | wc -l`
	printf "\n%12s %5s\n" "Total:" ${strs}
	printf "%12s %5s %s\n" "Unfinished:" ${unfi} "[`expr \( ${strs} - ${unfi} \) \* 100 / ${strs}`% done]"
	printf "%12s %5s %s\n\n" "Obsolete:" ${obso} "[`expr ${obso} \* 100 / ${strs}`%]"
fi
