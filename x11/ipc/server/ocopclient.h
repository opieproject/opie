#ifndef OPIE_OCOP_CLIENT_H
#define OPIE_OCOP_CLIENT_H

#include <qcstring.h>
#include <qsocketnotifier.h>

struct OCOPClient {
    int fd;
    QSocketNotifier* notify;
};

#endif
