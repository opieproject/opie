#!/bin/bash
touch infowin.cpp
for f in *lib.zmak; do make -f $f; done
for f in *.zmak; do make -f $f; done
for f in *.zmak; do make -f $f; done
cp /home/tim/codecreader/uqtreader*.desktop /opt/Qtopia/apps/Applications/
cp /home/tim/codecreader/pics/* /opt/Qtopia/pics/opie-reader/
cp /home/tim/codecreader/uqtreader*.desktop $QTDIR/apps/Applications/
cp /home/tim/codecreader/opie-reader.desktop $QTDIR/apps/Applications/
cp /home/tim/codecreader/pics/* $QTDIR/pics/opie-reader/
cp HTMLentities $QTDIR/plugins/reader/data/
rm $QTDIR/help/html/*reader*.html
cp /home/tim/codecreader/help/*.html $QTDIR/help/html/
rm -f $QTDIR/lib/libreader*.so
rm -f $QTDIR/lib/libreader*.so.1
for f in $QTDIR/lib/libreader*.1.0.0 ; do mv -f $f $QTDIR/lib/`basename $f .1.0.0`.1 ; done
rm -f $QTDIR/lib/libreader*.so.1.*

/home/tim/bin/zstrip.sh $QTDIR/bin/uqtreader
for f in $QTDIR/lib/libreader* ; do /home/tim/bin/zstrip.sh $f; done

for SUBDIR in codecs support filters
do
  CDIR=$QTDIR/plugins/reader/$SUBDIR
  rm -f $CDIR/*.so
  for f in $CDIR/*.1.0.0 ; do mv -f $f $CDIR/`basename $f .1.0.0` ; done
  rm -f $CDIR/*.so.*
  for f in $CDIR/*.so ; do /home/tim/bin/zstrip.sh $f ; done
done
rm -f opie-reader_*.ipk
rm -f full/opie-reader_*.ipk
rm -f partial/opie-reader_*.ipk
mkipks opie-reader.control
cp opie-reader_*.ipk full/
for f in opie-reader_*.ipk 
  do
  cp  $f full/`basename $f _arm.ipk`-`date +%y%m%d`_arm.ipk
done
rm $QTDIR/plugins/reader/support/libpluckerdecompress.so
rm $QTDIR/plugins/reader/codecs/libArriereGo.so
rm $QTDIR/plugins/reader/codecs/libNEF.so
mkipks opie-reader.control
cp opie-reader_*.ipk partial/
for f in opie-reader_*.ipk 
  do
  cp  $f partial/`basename $f _arm.ipk`-`date +%y%m%d`_arm.ipk
done
