#!/bin/sh
BDIR=/opt/QtPalmtop/etc/networktemplates
[ ! -z "$1" -a -f ${BDIR}/$1.mac ] && ifconfig $1 hw ether `cat ${BDIR}/$1.mac`
exit 0
