#!/bin/sh

# diskfree.sh - a demonstration of opie-sh
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
# 20020524-5 - bugfix
# 20020524-1 - initial release

OPIE_SH=/opt/QtPalmtop/bin/opie-sh

# there must be a better way to do this with sed, but i had no time
# time to look into this yet...

( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/fsmounter.png>"
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
) 	| $OPIE_SH -t "Space left" -f

rm -f /tmp/qcop-msg-diskfree.sh
