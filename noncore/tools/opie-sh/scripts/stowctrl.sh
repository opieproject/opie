#!/bin/sh

#stowctrl.sh - modified usbnetctrl.sh from gonz by spiralman spiralman@softhome.net
# usbnetctrl.sh - a demonstration of opie-sh
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
# 20020524-1 - code beautification, added icons and reset
# 20020517-2 - bugfixed fullscreen, added info about ifconfig
# 20020517-1 - added about, fullscreen and icon

OPIE_SH=/opt/QtPalmtop/bin/opie-sh

######################################################################
# subroutines

about() {
	( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/stowctrl.png>"
	  echo "<h3>About</h3>"
	  echo "This little App should make it easy for you to "
	  echo "activate / deactivate the stowaway keyboard (on ipaq)"
	  echo "<p>"
        )  | $OPIE_SH -t "stowaway control" -f  &
	SCREENCLEAN=$!
	sleep 1
}

startup() {
	  echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/stowctrl.png>" > /tmp/sout
	    echo "<h3>stowaway up</h3>" >> /tmp/sout
	modprobe h3600_stowaway >> /tmp/sout
	cat /dev/stowaway &
	echo $! > /tmp/stowcatpid
	 $OPIE_SH -t Output -f  /tmp/sout
	rm /tmp/sout
}

stopit() {
	( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/stowctrl.png>"
	  echo "<h3>stowaway down</h3>"
	read THECAT < /tmp/stowcatpid
	kill $THECAT
	rmmod h3600_stowaway
         ) 2>&1 | $OPIE_SH -t Output -f 
}


cleanup() {
	kill  $SCREENCLEAN
	rm -f /tmp/qcop-msg-stowctrl.sh
}


yesorno() {
	$OPIE_SH -m -t "Stowaway Up/Down" -M "What to do with the Stowaway: " \
	        -g -0 Up -1 Down -2 Restart
	RETURNCODE=$?

	case $RETURNCODE in
	-1) echo error... 
	    cleanup
	    exit   ;;
	0) startup ;;
	1) stopit  ;;
	2) stopit ; sleep 1 ; startup ;;
	esac
}

######################################################################
# main

about
yesorno
cleanup
