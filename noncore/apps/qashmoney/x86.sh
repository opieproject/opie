# !/bin/bash
CROSSCOMPILE=/opt/Embedix/tools
QTDIR=/opt/Qtopia
QPEDIR=/opt/Qtopia
PATH=$PATH:$QTDIR/bin:$QPEDIR/bin:/opt/Embedix/tools/bin:$QTDIR/tmake/bin
TMAKEPATH=/opt/Qtopia/tmake/lib/qws/linux-x86-g++/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$QTDIR/lib:/usr/lib
INCPATH=/usr/include/g++-3
export QPEDIR QTDIR PATH TMAKEPATH LD_LIBRARY_PATH PS1
echo "Altered environment for Sharp Zaurus Development x86"
