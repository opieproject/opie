#!/bin/sh

# wavelanchooser.sh - a demonstration of opie-sh
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
# 20020528-1 - fixed filename in cleanup
# 20020526-1 - rudimental but working
# 20020524-1 - initial working release planned
#

OPIE_SH=opie-sh

about() {
	( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/qtwavelan.png>"
	  echo "<h3>About</h3>"
	  echo "This little App is for changing the location setting"
	  echo "for your wireless card. <br>"
	  echo "actually it only starts /etc/init.d/[home|work] and can"
	  echo "be easily turned into a quick program to start the service"
          echo "or init script you use most."
	  echo "<p>"
        )  | $OPIE_SH -t wavelanchooser -f  &
	SCREENCLEAN=$!
	sleep 1
}

cleanup() {
	kill  $SCREENCLEAN $SCREENCLEAN2
	rm -f /tmp/qcop-msg-opie-sh-wavelanchooser.sh
}


about

# choose if work or home
$OPIE_SH -m -t "network-settings dialog" 		\
	-M "which configuration do you want ? "  \
	  -w -0 home -1 work -g

if [ "$?" = "0" ] 
then	DECISION=home
else	DECISION=work
fi

( echo "you decided to choose $DECISION configuration. "
  echo "remember to reinsert card."
  /etc/init.d/$DECISION start
) | $OPIE_SH -t "confirmation" -f \


cleanup
