# store/set MAC address for given interface
NS2MacStore=/etc/NS2

[ ! -d ${NS2MacStore} ] && mkdir ${NS2MacStore}

if [ -z "$1" ]
then 
  exit 0
fi

if [ -f ${NS2MacStore}/$1.mac ] 
then
  # set this mac
  /sbin/ifconfig $1 hw ether `cat ${NS2MacStore}/$1.mac`
else
  # remember current mac for all times
  X=`/sbin/ifconfig $1 | grep HWaddr`
  X=${X#*HWaddr } # strip till HWaddr
  X=${X%% *} # remove trailing spaces
  echo ${X} > ${NS2MacStore}/$1.mac
fi
exit 0
