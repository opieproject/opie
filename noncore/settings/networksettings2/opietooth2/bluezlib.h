#ifndef BLUEZLIB_H
#define BLUEZLIB_H

#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

extern "C" {
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/bnep.h>
#include <bluetooth/rfcomm.h>
}

#define BT_DISABLED 0
#define BT_ENABLED  1
#define BT_UNKNOWN  2
#define BTVALUE(x)       ((x)?BT_ENABLED:BT_DISABLED)

#endif
