#!/bin/sh

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
	( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/usbnetctrl.png>"
	  echo "<h3>About</h3>"
	  echo "This little App should make it easy for you to "
	  echo "activate / deactivate the usbnet-driver (on ipaq)"
	  echo "<p>"
	  echo "To use it at first, you need to change the ifconfig"
	  echo "line in /opt/QtPalmtop/bin/usbnetctrl.sh."
        )  | $OPIE_SH -t "usbnet control" -f  &
	SCREENCLEAN=$!
	sleep 1
}

startup() {
	  ( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/usbnetctrl.png>"
	    echo "<h3>usbnet up</h3>"
	    modprobe usb-eth
	    ) 2>&1 | $OPIE_SH -t Output -f
	    
	    $OPIE_SH -m -I -t "Usbnet Control" -M "Please connect the<br>ipaq to the cradle<br>and press OK"

	 ( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/usbnetctrl.png>"
	    echo "<h3>usbnet up</h3>"
	    ifconfig usbf up 192.168.0.1 netmask 255.255.255.0  

	  ) 2>&1 | $OPIE_SH -t Output -f
	  if [ -f /root/masq.sh ]
	  then sh /root/masq.sh
	  fi 
}

stopit() {
	( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/usbnetctrl.png>"
	  echo "<h3>usbnet down</h3>"
          ifconfig usbf down
	  sleep 1
	  rmmod usb-eth
	  rmmod sa1100usb_core
        ) 2>&1 | $OPIE_SH -t Output -f 
}


cleanup() {
	kill  $SCREENCLEAN
	rm -f /tmp/qcop-msg-usbnetctrl.sh
}


yesorno() {
	$OPIE_SH -m -t "USB-Net start/stop" -M "What to do with usbnet: " \
	        -g -0 Start -1 Stop -2 Restart
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
