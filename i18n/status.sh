#/bin/sh
arg=$1
if [ x${arg}x = xx ]
then
	arg=.
fi
echo "number of strings: "
strs=`find $arg -name "*.ts" -exec cat {} \; | grep translation | wc -l`
echo $strs
echo "unfinished: "
unfi=`find $arg -name "*.ts" -exec cat {} \; | grep translation | grep type=\"unfinished\" | wc -l`
echo $unfi `expr \( $strs - $unfi \) \* 100 / $strs`%
echo "obsolete: "
obso=`find $arg -name "*.ts" -exec cat {} \; | grep translation | grep type=\"obsolete\" | wc -l`
echo $obso `expr \( $strs - $obso \) \* 100 / $strs`%
