#!/bin/sh

# hostlist.sh - a demonstration of opie-sh
#
# Copyright (C) 2002 gonz@directbox.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# 20020524-1 - icon to hostlist
# 20020517-1 - added icon, changed name in launcher
#

CFG=/opt/QtPalmtop/share/config/hostlist.cfg
OPIE_SH=opie-sh

hostlist() {
(	echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/hostlist.png>"
	echo "<h3>Hostlist</h3><br>"
	HOSTLIST=`cat $CFG | grep -v '#'`
	for HOST in $HOSTLIST
	do
		PT=` ping -c 1 $HOST | grep avg|cut -d " " -f4-`
		if [ "$PT" != "" ] 
		then echo "<b><font color=#00ff00> "
		     echo "$HOST is up<br></font></b>[$PT]<br>"
		else echo "<b><font color=#ff0000> "
		     echo "$HOST is down<br></font></b>[$PT]<br>"
		fi
	done
)       | $OPIE_SH -t "Which are up ?" -f 


}

cleanup() {
	kill  $SCREENCLEAN
	rm -f /tmp/qcop-msg-hostlist.sh
}

about() {
	( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/hostlist.png>"
	  echo "<h3>About</h3>"
	  echo "This little App should make it possible for you to "
	  echo "check some hosts for availability."
	  echo "<p>"
        )  | $OPIE_SH -t hostlist -f  &
	SCREENCLEAN=$!
	sleep 1
}

#####################################################################
#
# main

about

# ask to: start or edit list
$OPIE_SH -m -g -t Hostlist -M "Welcome!" -0 Start -1 "Edit Hostlist"
RETURNCODE=$?
case $RETURNCODE in
-1)	echo died unexpectedly... | $OPIE_SH -f
	cleanup
	exit ;; 
1)      textedit $CFG 
	cleanup ;;
0)	if [ -f $CFG ]
	then	hostlist
	else 	echo www,handhelds.org >$CFG ; hostlist
	fi
	cleanup 
	;;
esac
