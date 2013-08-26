#!/bin/sh
if /bin/pidof qpe > /dev/null ; then
    # Notify Opie cardapplet of pc card events
    export QTDIR=/usr/lib/opie
    export QPEDIR=/usr/lib/opie
    /usr/bin/qcop QPE/Card stabChanged\(\)
fi
