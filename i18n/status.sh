#/bin/sh
arg=$1
echo "number of strings: "
grep translation $arg/*.ts | wc -l
echo "unfinished: "
grep translation $arg/*.ts | grep type=\"unfinished\" | wc -l
echo "obsolete: "
grep translation $arg/*.ts | grep type=\"obsolete\" | wc -l
