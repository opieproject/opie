#!/bin/sh
getmac() {
  echo $5
}

BDIR=/opt/QtPalmtop/etc/networktemplates
if [ -f ${BDIR}/$1.mac ]
then
# store
ifconfig $1 hw ether `cat ${BDIR}/$1.mac`
else
# set
mkdir -p $1
i=`ifconfig $1 | grep -i hwaddr`
getmac ${i} > ${BDIR}/$1.mac
fi
exit 0
