#/bin/sh
arg=$1
foo=$1
if [ x${arg}x = xx ]
then
	arg=.
	foo="all"
fi

echo "Status of $foo"
echo

#general figures
echo "Status of $foo"
echo

echo "number of strings: "
strs=`find $arg -name "*.ts" -exec cat {} \; | grep translation | wc -l`
echo $strs
echo "unfinished: "
unfi=`find $arg -name "*.ts" -exec cat {} \; | grep translation | grep type=\"unfinished\" | wc -l`
echo $unfi", that means" `expr \( $strs - $unfi \) \* 100 / $strs`% "are done"
echo "obsolete: "
obso=`find $arg -name "*.ts" -exec cat {} \; | grep translation | grep type=\"obsolete\" | wc -l`
echo $obso `expr \( $strs - $obso \) \* 100 / $strs`%

#from here on we will only look at the core-parts. All numbers have to be 0% if
#the specific language will be in the official release.
echo
echo "Core:"
core_today=`grep translation $arg/today.ts | grep type=\"unfinished\" | wc -l`
core_todo=`grep translation $arg/todolist.ts | grep type=\"unfinished\" | wc -l`
core_addressbook=`grep translation $arg/addressbook.ts | grep type=\"unfinished\" | wc -l`
core_datebook=`grep translation $arg/datebook.ts | grep type=\"unfinished\" | wc -l`
libopie=`grep translation $arg/libopie.ts | grep type=\"unfinished\" | wc -l`
_core_today=`grep translation $arg/today.ts | wc -l`
_core_todo=`grep translation $arg/todolist.ts | wc -l`
_core_addressbook=`grep translation $arg/addressbook.ts | wc -l`
_core_datebook=`grep translation $arg/datebook.ts | wc -l`
_libopie=`grep translation $arg/libopie.ts | wc -l`

echo "Today:" $core_today", that means" `expr $core_today \* 100 / $_core_today`% "needs to be done"
echo "Datebook:" $core_datebook", that means" `expr $core_datebook \* 100 / $_core_datebook`% "needs to be done"
echo "Todo:" $core_todo", that means" `expr $core_todo \* 100 / $_core_todo`% "needs to be done"
echo "Addressbook:" $core_addressbook", that means" `expr $core_addressbook \* 100 / $_core_addressbook`% "needs to be done"
echo "libopie:" $libopie", that means" `expr $libopie \* 100 / $_libopie`% "needs to be done"
