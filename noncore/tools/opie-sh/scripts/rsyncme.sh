#!/bin/sh

# rsyncme.sh - a demonstration of opie-sh
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
# 20020517-1 - added icon, changed name in launcher
#

CFG=/opt/QtPalmtop/share/config/rsyncme.cfg
OPIE_SH=opie-sh

update_cfg() {
	# check if dir is in CFG and if not add it
	if [ "`grep -c $DIR $CFG`" = "0" ]
	then	echo $DIR >>$CFG
	fi
}

ask_dirs() {
	# get source and destination dir
	DIR=`$OPIE_SH -g -i -t "Choose Source Directory" -l -L DIR: -E -F $CFG `
	update_cfg
	SRCDIR=$DIR

	DIR=`$OPIE_SH -g -i -t "Choose Destination Directory" -l -L DIR: -E -F $CFG `
	update_cfg
	DESTDIR=$DIR
}

ask_options() {
	# now ask for options
	OPTIONS="--archive --verbose --checksum --recursive --update --links --partial --dry-run --rsh=ssh --existing --compress --recursive"
	OUTPUT=`( for OPTION in $OPTIONS
	          do
			echo "$OPTION"
	          done ) | $OPIE_SH -t "Select options" -g -i -b -F `
}

rsync_output() {
	( echo "<h3>Output of your rsync:</h3>"
	  echo "command: rsync $OUTPUT $SRCDIR $DESTDIR"
	  echo "<pre>"
	  rsync $OUTPUT $SRCDIR $DESTDIR 2>&1
	  echo "</pre>done."
	) | $OPIE_SH -t "Result:" -f
}

cleanup() {
	kill  $SCREENCLEAN
	rm -f /tmp/qcop-msg-rsyncme.sh
}

show_dirs() {
	( echo "<h3>Directorys chosen for sync:</h3>"
	  echo "<ul><li>$SRCDIR<li>$DESTDIR</ul>" ) \
		| $OPIE_SH -t "Got directories" -f
}

about() {
	( echo "<img src=/opt/QtPalmtop/pics/opie-sh-scripts/rsyncme.png>"
	  echo "<h3>About</h3>"
	  echo "This little App should make it possible for you to "
	  echo "easily choose the wanted options for an rsync process."
	  echo "<p>"
        )  | $OPIE_SH -t rsync-helper -f  &
	SCREENCLEAN=$!
	sleep 1
}




#####################################################################
#
# main

about

# ask to: start rsync, edit dirlist, show about
$OPIE_SH -m -g -t RsyncMe -M "Welcome!" -0 Start -1 Dirlist 
RETURNCODE=$?
case $RETURNCODE in
-1)	echo died unexpectedly... | $OPIE_SH -f
	cleanup
	exit ;; 
1)      textedit $CFG 
	cleanup ;;
0)	if [ -f $CFG ]
	then	ask_dirs
		show_dirs
		ask_options
		rsync_output
	else 	( echo no config file seems to exist - see About... ) \
		   | $OPIE_SH -t "SORRY..." -f
	fi
	cleanup 
	;;
esac
