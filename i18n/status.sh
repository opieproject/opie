#!/bin/sh

#
# Function:
#   showStringStats <title> <file>
#
# Parameters:
#   <title>   Title to be displayed.
#   <file>    Name of file to be counted.
#
function showStringStats ()
{
	echo -n "$1: "
	if [ -e "$2" ]; then
		csTotal=`grep translation "$2" | wc -l`
		csUnfinished=`grep translation "$2" | grep type=\"unfinished\" | wc -l`
		echo ${csUnfinished}", that means `expr ${csUnfinished} \* 100 / ${csTotal}`% needs to be done."
	else
		echo "'$2' is missing. No stats available!"
	fi
}

#
# Main
#
arg=$1
foo=$1
if [ "x${arg}x" = "xx" ]; then
	arg="."
	foo="all"
fi

#
# General figures
#
echo -e "Status of ${foo}:\n"

echo -n "Number of strings: "
strs=`find ${arg} -name "*.ts" -exec grep translation {} \; | wc -l`
echo ${strs}

echo -n "Unfinished: "
unfi=`find ${arg} -name "*.ts" -exec grep translation {} \; | grep type=\"unfinished\" | wc -l`
echo ${unfi}", that means `expr \( ${strs} - ${unfi} \) \* 100 / ${strs}`% are done."

echo -n "Obsolete: "
obso=`find ${arg} -name "*.ts" -exec grep translation {} \; | grep type=\"obsolete\" | wc -l`
echo ${obso}" (`expr ${obso} \* 100 / ${strs}`%)"

#
# from here on we will only look at the core-parts. All numbers have to be 0% if
# the specific language will be in the official release.
#
echo -e "\nCore:\n"

showStringStats "Addressbook" "${arg}/addressbook.ts"
showStringStats "Datebook" "${arg}/datebook.ts"
showStringStats "libopie" "${arg}/libopie.ts"
showStringStats "Today" "${arg}/today.ts"
showStringStats "Todo" "${arg}/todolist.ts"
