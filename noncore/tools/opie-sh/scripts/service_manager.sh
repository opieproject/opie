#!/bin/sh

# service_manager.sh - a demonstration of opie-sh
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
# 20020524-1 - added icon to statusinfo...
# 20020518-1 - optix
# 20020517-1 - added about and fullscreen
#

OPIE_SH=/opt/QtPalmtop/bin/opie-sh
INITPATH=/etc/init.d

######################################################################
# subroutines

about() {
	( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/service_manager.png>"
	  echo "<h3>About</h3>"
	  echo "This little App should make it possible for you to "
	  echo "easily start and stop services in /etc/init.d."
	  echo "<p>"
        )  | $OPIE_SH -t service-manager -f  &
	SCREENCLEAN=$!
	sleep 1
}

cleanup() {
	kill  $SCREENCLEAN
	rm -f /tmp/qcop-msg-service_manager.sh
}

get_action() {
        # ask what to do (start/stop/status)
        #
        $OPIE_SH -m -t "Select Action"                               \
                    -M "which action do you want to do to $SERVICE ?<br>" \
                    -g -0 start -1 stop -2 status
        RETURNCODE=$?

        case $RETURNCODE in
                -1) echo unexpected input detected, exiting.
                    exit          ;;
                0)  ACTION=start  ;;
                1)  ACTION=stop   ;;
                2)  ACTION=status ;;
        esac
}

status_disclaimer() {
        # tell that init scripts are not too standard
        #
        cat | $OPIE_SH -f <<EOT
<img src=/opt/QtPalmtop/pics/opie-sh-scripts/service_manager.png>
<h3>status has been disabled</h3>

        the status service has been disabled
        because it is not supported by the 
        most init scripts that come with 
        familiar ...<p>

        perhaps it will be replaced with 
        something more useful in the future...
EOT
}

select_service() {
        # present service list and choose
        #
        cd $INITPATH/
        SERVICE=` ls -1 | $OPIE_SH -i -l \
                      -g -t "Select Service" \
                      `
        cd -
}

######################################################################
# main

about
select_service
get_action

if [ "$SERVICE" = "" ]
then    cleanup ; exit
fi

if [ "$ACTION" = "status" ]
then    status_disclaimer
else    ( echo "<h3>Output of $INITPATH/$SERVICE $ACTION:</h3>"
	  echo "<pre>"
          $INITPATH/$SERVICE $ACTION
          echo "</pre><p>"
          echo done.
        ) | $OPIE_SH -f
fi

cleanup

