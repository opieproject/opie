VERSION=2-1.0.2

TGZDIR=$PWD

# script to make a .tgz distributable for the Wellenreiter X11 Standalone Version

find . -name "*.o"|xargs rm -f
find . -name "Makefile"|xargs rm -f
find . -name "*moc*"|xargs rm -f
#TMPFILE=`mktemp -d -q /tmp/build.XXXXXX`
TMPFILE=/tmp/build
#if [ $? -ne 0 ]; then
#        echo "$0: Can't create temp file, exiting..."
#        exit 1
#fi

TMPDIR=$TMPFILE/wellenreiter$VERSION
OUTPUT=$TMPDIR/output

mkdir -p $TMPDIR/libopie2/opiecore
mkdir -p $TMPDIR/libopie2/opieui
mkdir -p $TMPDIR/libopie2/opienet
mkdir -p $TMPDIR/include/opie2
mkdir -p $OUTPUT/share/wellenreiter/pics
mkdir -p $OUTPUT/share/wellenreiter
cp -dfR gui $TMPDIR
cp -dfR lib $TMPDIR
cp -dfR build README wellenreiter.pro $TMPDIR
cp -dfR $OPIEDIR/pics/wellenreiter/* $OUTPUT/share/wellenreiter/
cp -dfR $OPIEDIR/etc/manufacturers $OUTPUT/share/wellenreiter/

# take care about sourcefiles from libopie2

cp -dfR $OPIEDIR/libopie2/opienet/*.* $TMPDIR/libopie2/opienet
cp -dfR $OPIEDIR/libopie2/opiecore/*.* $TMPDIR/libopie2/opiecore
cp -dfR $OPIEDIR/libopie2/opieui/olistview.* $TMPDIR/libopie2/opieui

# make includes

pushd $TMPDIR/include/opie2
ln -sf ../../libopie2/opieui/*.h .
ln -sf ../../libopie2/opienet/*.h .
ln -sf ../../libopie2/opiecore/*.h .
popd

find $TMPDIR -name "CVS"|xargs rm -rf

pushd $TMPFILE
tar czf $TGZDIR/wellenreiter$VERSION.tgz wellenreiter$VERSION
popd

