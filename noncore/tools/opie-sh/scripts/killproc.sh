#!/bin/sh

# killproc.sh - a demonstration of opie-sh
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
# 20020524-1 - code optix
# 20020519-1 - added icon to output
# 20020517-1 - nicer fullscreen, added about, tried icon

OPIE_SH=/opt/QtPalmtop/bin/opie-sh

######################################################################
# subroutines

about() {
	( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/killproc.png>"
	  echo "<h3>About</h3>"
	  echo "This little util sends the TERM signal "
	  echo "to the specified process or process group. "
	  echo "The TERM signal will kill processes which  "
	  echo "do not catch this signal. "
	  echo "<p> "
        )  | $OPIE_SH -t opie-killproc -f  &
	SCREENCLEAN=$!
	sleep 1
}

cleanup() {
	kill  $SCREENCLEAN
	rm -f /tmp/qcop-msg-killproc.sh
}

select_process() {
        # present process list and choose
        #
	PROCESS=`ps -eaf | cut -b 10-15,40- 	\
                         | cut -b -40  		\
                         | grep -v "TIME CMD" 	\
                         | $OPIE_SH -i -l -g   	\
                                  -t "Select Process" `
}

######################################################################
# main

about
select_process

PROCESS=`echo $PROCESS | cut -b -4`


( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/killproc.png>"
  echo "<h3>Output of kill $PROCESS</h3>"
  echo "<pre>"
  kill $PROCESS 2>&1
  echo "</pre><p>"
  echo done.
) | $OPIE_SH -f

cleanup
