#unset PREBIND
unset LD_PREBIND
unset LD_PREBIND_ALLOW_OVERLAP
unset LD_SEG_ADDR_TABLE
unset QMAKESPEC
# Needed to activate Prebinding capababilites (since 10.2 and newer)
MACOSX_DEPLOYMENT_TARGET=10.2
#Set Vars for QT 2.3.2
THISDIR=`pwd`
QTDIR=$THISDIR/qt-2.3.2
PATH=$QTDIR/bin:$PATH
DYLD_LIBRARY_PATH="$QTDIR/lib"

#Set vars for QTE
QPEDIR=$THISDIR/qte-2.3.7
QTDIR=$QPEDIR
DYLD_LIBRARY_PATH="$QTDIR/lib":$DYLD_LIBRARY_PATH

#Set vars for Opie
OPIEDIR=$THISDIR/opie
DYLD_LIBRARY_PATH="$OPIEDIR/lib":$DYLD_LIBRARY_PATH

#Set special compiler
# PATH=$OPIEDIR/../tools/bin:$PATH
# DYLD_LIBRARY_PATH=$OPIEDIR/../tools/bin:$DYLD_LIBRARY_PATH

#Show special environment

PS1='\u:(QT):\w\$ '

export QTDIR QPEDIR OPIEDIR PATH DYLD_LIBRARY_PATH PS1 MACOSX_DEPLAYMENT_TARGET

# Now start virtual framebuffer device..
qvfb -depth 16 &