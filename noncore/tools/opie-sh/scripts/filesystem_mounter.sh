#!/bin/sh

# filesystem_mounter.sh - a demonstration of opie-sh
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
# 20020524-2 - using df from diskfree now
# 20020524-1 - just code optix
# 20020519-1 - added information screen with manpage excerpt, added 
#              better returncode-checking
# 20020517-2 - fixed output bug
# 20020517-1 - nicer fullscreen, added about, tried icon
#

OPIE_SH=/opt/QtPalmtop/bin/opie-sh

######################################################################
# subroutines

about() {
	( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/fsmounter.png>"
	  echo "<h3>About</h3>"
	  echo "This little App should make it possible for you to "
	  echo "easily mount and unmount filesystems from /etc/fstab"
	  echo "<p>"
        )  | $OPIE_SH -t fsmounter -f  &
	SCREENCLEAN=$!
	sleep 1
}

beforemount() {
        ( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/fsmounter.png>"
          echo "<h3>mount or unmount $MOUNTPOINT ?</h3>"
	  echo "<b>excerpt from MOUNT(8) manpage:</b><p>"
	  echo "All  files accessible in a Unix system are arranged in one
		big tree, the file hierarchy, rooted at  <b>/</b>.   These  files
		can  be spread out over several devices. The <b>mount</b> command
		serves to attach the file system found on some  device  to
		the  big file tree. Conversely, the <b>umount(8)</b> command will
		detach it again."
	  echo "<p>"
	)  | $OPIE_SH -t fsmounter -f  &
	SCREENCLEAN2=$!
	sleep 1
}


cleanup() {
	kill  $SCREENCLEAN $SCREENCLEAN2
	rm -f /tmp/qcop-msg-filesystem_mounter.sh
}

get_action() {
# ask what to do (start/stop/status)
#
$OPIE_SH -m -t "Select Action"                                  \
	    -M "action for $MOUNTPOINT ?" \
	    -g -0 mount -1 umount
RETURNCODE=$?

case $RETURNCODE in
	-1|255) echo unexpected input detected, exiting. | $OPIE_SH -f
	    cleanup ;;
	0)  ACTION=mount  ;;
	1)  ACTION=umount ;;
esac
kill $SCREENCLEAN2
}

select_mountpoint() {
# present service list and choose
#
MOUNTPOINT=` cat /etc/fstab | cut -f1               \
		| cut -d " " -f1                    \
		| egrep -v "(tmpfs|proc|devpts|^#)" \
		| $OPIE_SH -i -l 		    \
			  -g -t "SELECT MOUNTPOINT" \
			  -L "select fs: " `
}

######################################################################
# main

about
select_mountpoint
beforemount
get_action

if [ "$MOUNTPOINT" = "" ]
then    echo "ugly error...." | $OPIE_SH -f ; cleanup
fi

( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/fsmounter.png>"
  echo "<h3>Output of $ACTION $MOUNTPOINT :</h3>"
  echo "<pre>"
  $ACTION $MOUNTPOINT  2>&1
  echo "</pre><p>"
  echo "<h3>diskfree</h3> how much space is left ?<br>"
  echo "<p><table>"

  df -Ph					\
	| grep -v  "Mounted on"			\
	| sed 's/  / /g' 			\
	| sed 's/  / /g' 			\
	| sed 's/  / /g' 			\
	| sed 's/  / /g' 			\
	| sed 's/  / /g' 			\
	| sed 's/  / /g' 			\
	| sed "s/  / /g"        		\
	| cut -d " " -f4-			\
	| sed 's/ /<\/td><td>/g'		\
	| sed 's/$/<\/td><\/tr>/'		\
	| sed 's/^/<tr><td>/' ; echo "</table>" \
) | $OPIE_SH -f

cleanup
