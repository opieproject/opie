#!/bin/sh

FILES="liquid.h liquid.cpp plugin.cpp"

echo -n "Creating QPL diff for liquid ... "

cat >opie-liquid.diff <<EOT
This is a patch to mosfet's liquid 0.7.
Features:
 - Qt-only
 - works with Qt/E on QPE/OPIE
 
EOT

for i in $FILES; do 
  tar xzOf mosfet-liquid0.7.tar.gz "mosfet-liquid0.7/widget-engine/$i" | diff -bu3 - "$i" >>opie-liquid.diff
done

echo "done"

exit 0