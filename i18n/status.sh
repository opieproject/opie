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

echo "number of strings: "
strs=`find $arg -name "*.ts" -exec cat {} \; | grep translation | wc -l`
echo $strs
echo "unfinished: "
unfi=`find $arg -name "*.ts" -exec cat {} \; | grep translation | grep type=\"unfinished\" | wc -l`
echo $unfi", that means" `expr \( $strs - $unfi \) \* 100 / $strs`% "are done"
echo "obsolete: "
obso=`find $arg -name "*.ts" -exec cat {} \; | grep translation | grep type=\"obsolete\" | wc -l`
echo $obso `expr \( $strs - $obso \) \* 100 / $strs`%
