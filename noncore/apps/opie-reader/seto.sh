red='\e[0;31m'
RED='\e[1;31m'
blue='\e[0;34m'
BLUE='\e[1;34m'
cyan='\e[0;36m'
CYAN='\e[1;36m'
NC='\e[0m'              # No Color

if test -z $OLDPATH; then
    export OLDPATH=$PATH
fi

if test -z $OLD_LD_LIBRARY_PATH; then
    export OLD_LD_LIBRARY_PATH=$LD_LIBRARY_PATH
fi

export QPEDIR=/opt/Qtopia/opie
export QTDIR=/opt/Qtopia/opie
export PATH=$QTDIR/bin:/usr/local/arm/3.3/bin:$OLDPATH
export TMAKEPATH=/opt/Qtopia/tmake/lib/qws/linux-opie-g++
#export LD_LIBRARY_PATH=$QTDIR/lib:$OLD_LD_LIBRARY_PATH

PS1="[Opie]\W> "

echo -e \\n${RED}Opie${NC} environment configured\\n
