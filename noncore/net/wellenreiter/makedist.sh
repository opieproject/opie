VERSION=2-1.0rc2

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

# take care about sourcefiles

FILES="libopie2/opienet/onetwork.h libopie2/opienet/onetwork.cpp \
      libopie2/opienet/opcap.h libopie2/opienet/opcap.cpp libopie2/opienet/802_11_user.h \
      libopie2/opienet/onetutils.h libopie2/opienet/onetutils.cpp \
      libopie2/opienet/omanufacturerdb.h libopie2/opienet/omanufacturerdb.cpp \
      libopie2/opieui/olistview.cpp libopie2/opieui/olistview.h"

for i in $FILES
    do cp -dfR $OPIEDIR/$i $TMPDIR/$i
done

# make includes
pushd $TMPDIR/include/opie2
ln -sf ../../libopie2/opieui/*.h .
ln -sf ../../libopie2/opienet/*.h .
popd

find $TMPDIR -name "CVS"|xargs rm -rf

pushd $TMPFILE
tar czf $TGZDIR/wellenreiter$VERSION.tgz wellenreiter$VERSION
popd

