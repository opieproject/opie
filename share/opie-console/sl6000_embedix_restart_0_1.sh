#!/bin/sh

# Use egrep to fid out if we need to comment /etc/sync/serialctl
# from the inittab
/bin/egrep -q '^#pd:[0-9]:respawn:/etc/sync/serialctl' /etc/inittab 2>/dev/null

# If egrep did not find an enabled /etc/sync/serialctl in /etc/inittab,
# exit now (nothing to do)
if [ $? -ne 0 ]; then
  exit 0
fi

# This "cp -af" is to get the perms correct on /etc/inittab.tmp
/bin/cp -af /etc/inittab /etc/inittab.tmp

# This sed comments out the "etc/sync/serialctl" line
/bin/sed 's%^#\(pd:[0-9]:respawn:/etc/sync/serialctl\)%\1%' /etc/inittab > /etc/inittab.tmp

# We have not yet modified /etc/inittab, so to stay on the safe side
# only modify it if the sed succeeded at what we asked it to do.
if [ $? -eq 0 ]; then
  # This "cp -af" puts the new /etc/inittab into place
  /bin/cp -af /etc/inittab.tmp /etc/inittab
  # Unlink the tmp file
  /bin/rm /etc/inittab.tmp
  # HUP init
  /bin/kill -HUP 1
fi

