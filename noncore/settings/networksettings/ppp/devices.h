#ifndef _DEVICES_H_
#define _DEVICES_H_

#include "kpppconfig.h"

static const char *devices[] = {
#ifdef __FreeBSD__
  "/dev/cuaa0",
  "/dev/cuaa1",
  "/dev/cuaa2",
  "/dev/cuaa3",
  "/dev/cuaa4",	
#elif defined(__NetBSD__)
  "/dev/tty00",                /* "normal" modem lines */
  "/dev/tty01",
  "/dev/tty02",
  "/dev/tty03",
  "/dev/dty00",                /* Dial out devices */
  "/dev/dty01",
  "/dev/dty02",
  "/dev/dty03",
  "/dev/ttyU0",                /* USB stuff modems */
  "/dev/ttyU1",
  "/dev/ttyU2",
  "/dev/ttyU3",
  "/dev/dtyU0",                /* USB stuff, too (dial out device) */
  "/dev/dtyU1",
  "/dev/dtyU2",
  "/dev/dtyU3",
#elif defined (__linux__)
  "/dev/modem",
  "/dev/ttyS0",
  "/dev/ttyS1",
  "/dev/ttyS2",
  "/dev/ttyS3",
#ifdef ISDNSUPPORT
  "/dev/ttyI0",
  "/dev/ttyI1",
  "/dev/ttyI2",
  "/dev/ttyI3",
#endif
  "/dev/usb/ttyACM0",          /* USB stuff modems */
  "/dev/usb/ttyACM1",
  "/dev/usb/ttyACM2",
  "/dev/usb/ttyACM3",
#elif defined(__svr4__)
  "/dev/cua/a",
  "/dev/cua/b",
  "/dev/ttya",
  "/dev/ttyb",
#endif
  0};

// default device number from the list above
const int DEV_DEFAULT = 0;

#endif
