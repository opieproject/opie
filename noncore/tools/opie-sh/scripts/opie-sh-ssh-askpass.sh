#!/bin/sh

# opie-sh-ssh-askpass -- ssh-askpass utility using opie-sh

OPIE_SH=opie-sh

exec $OPIE_SH -i -p -t "OpenSSH" -g -L "<SMALL>$@" 2>/dev/null

