#/bin/sh
echo "number of strings: "
grep translation go.ts | wc -l
echo "unfinished: "
grep translation go.ts | grep type=\"unfinished\" | wc -l
echo "obsolete: "
grep translation go.ts | grep type=\"obsolete\" | wc -l
