# this script is part of networksettings2 package
# purpose : set fixed MAC address for network devices that
#           have random addresses (like usb)
#

NS2MacStore=/etc/NS2

[ ! -d ${NS2MacStore} ] && mkdir ${NS2MacStore}

if [ -z "$1" ]
then
  exit 0
fi

if [ -f ${NS2MacStore}/$1.mac ]
then
  # set this mac
  X=`cat ${NS2MacStore}/$1.mac`
  if [ ! -z "$X" ]
  then
    /sbin/ifconfig $1 hw ether $X
  fi
else
  # remember current mac for all times
  X=`/sbin/ifconfig $1 | grep HWaddr`
  X=${X#*HWaddr } # strip till HWaddr
  X=${X%% *} # remove trailing spaces
  if [ ! -z "$X" ]
  then
    # valid mac address
    echo ${X} > ${NS2MacStore}/$1.mac
  fi
fi
exit 0
