#!/bin/sh

# launcher_refresh.sh - a demonstration of opie-sh
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

OPIE_SH=opie-sh

launcher_refresh() {
(	echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/launcher_refresh.png>"
	echo "<h3>Refresh successful!</h3><br>"
	echo the launcher has been refreshed.
	echo
	echo this is useful if you installed packages using ipk instead of
	echo oipkg. it is also helping if you are changing .desktop-files 
	echo often.
	echo "<p>" it also refreshes the input-methods.
)       | $OPIE_SH -t "launcher-refresh" -f 
}

cleanup() {
	rm -f /tmp/qcop-msg-launcher_refresh.sh
}

#####################################################################
#
# main

/opt/QtPalmtop/bin/qcop QPE/TaskBar "reloadInputMethods()"
#/opt/QtPalmtop/bin/qcop QPE/System "restart()"
/opt/QtPalmtop/bin/qcop QPE/System "linkChanged(QString)"

launcher_refresh
cleanup
