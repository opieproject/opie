#!/bin/sh

# logviewer.sh - a demonstration of opie-sh
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
# 20020526-1a - initial release...
#

OPIE_SH=/opt/QtPalmtop/bin/opie-sh
INITPATH=/etc/init.d

######################################################################
# subroutines

about() {
	( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/logviewer.png>"
	  echo "<h3>About</h3>"
	  echo "This little App should make it possible for you to "
	  echo "easily have a look at some of the syslogs. "
	  echo "<p>"
        )  | $OPIE_SH -t logviewer -f  &
	SCREENCLEAN=$!
	sleep 1
}

cleanup() {
	kill  $SCREENCLEAN 2>/dev/null
	rm -f /tmp/qcop-msg-logviewer.sh
}

choose_log() {
        # ask what to do (start/stop/status)
        #
        $OPIE_SH -m -t "Select Log"                               \
                    -M "which logfile are you interested in ?<br>" \
                    -g -0 "PROC" -1 "SYSLOG" 
        RETURNCODE=$?

        case $RETURNCODE in
                -1) echo unexpected input detected, exiting.
                    cleanup            ;;
                0)  LOGMETHOD=proc     ;;
                1)  LOGMETHOD=syslog   ;;
        esac
}

######################################################################
# main

about
choose_log

case $LOGMETHOD in
	proc) 	$OPIE_SH -m -t "Select Log" 		\
			 -M "which procfile ?<br>" 	\
			 -g -0 kmsg -1 "ksyms"
		RETURNCODE=$?
		case $RETURNCODE in
			-1) cleanup            ;;
			0)  

cat   /proc/kmsg  >/tmp/log.$$ &
PID=$!
sleep 2
kill $PID
( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/logviewer.png>"
  echo "<h3>/proc/kmsg</h3>"
  cat /tmp/log.$$
) | $OPIE_SH -t kmsg -f
rm -f /tmp/$$
cleanup ;;

			1)  

cat   /proc/ksyms >/tmp/log.$$ &
PID=$!
sleep 2
kill $PID
( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/logviewer.png>"
  echo "<h3>/proc/ksyms</h3>"
  cat /tmp/log.$$
) | $OPIE_SH -t ksyms -f 
rm -f /tmp/$$
cleanup ;;

			*)  cleanup ;;
		esac
		cleanup						;;

                syslog)	

if [ -f /var/log/messages ]
then
	$OPIE_SH -t syslog -f /var/log/messages
else
	echo "no syslogd installed or /var/log/messages inexistent." \
		| $OPIE_SH -t syslog -f 
fi
;;
                *)  cleanup ;;
        esac


cleanup

