#!/bin/bash
CROSSCOMPILE=/opt/Embedix/tools
QPEDIR=/opt/Qtopia/sharp
QTDIR=/opt/Qtopia/sharp
PATH=$QTDIR/bin:$QPEDIR/bin:$CROSSCOMPILE/bin:$PATH:$QPEDIR/tmake/bin
TMAKEPATH=/opt/Qtopia/tmake/lib/qws/linux-sharp-g++/
LD_LIBRARY_PATH=$QTDIR/lib:$LD_LIBRARY_PATH:
INCPATH=$INCPATH:/usr/include
export QPEDIR QTDIR PATH LD_LIBRARY_PATH TMAKEPATH PS1 INCPATH
echo "Altered environment for Sharp Zaurus Development ARM"
