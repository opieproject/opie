/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#define QPE_OWNAPM
#define QPE_HAVE_TOGGLELIGHT
#define QPE_NOCIBAUD
#define QPE_STARTMENU
#include <asm/sharp_apm.h>
#ifndef APM_IOC_BATTERY_BACK_CHK
#define APM_IOC_BATTERY_BACK_CHK       _IO(APM_IOC_MAGIC, 32)
#endif
#ifndef APM_IOC_BATTERY_MAIN_CHK
#define APM_IOC_BATTERY_MAIN_CHK       _IO(APM_IOC_MAGIC, 33)
#endif

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>


#include <sys/ioctl.h>
#include <asm/sharp_char.h>

#define QPE_HAVE_MEMALERTER

#define QPE_MEMALERTER_IMPL \
static void sig_handler(int sig) \
{ \
    switch (sig) { \
    case SIGHUP: \
  memstate = VeryLow; \
  break; \
    case SIGUSR1: \
  memstate = Normal; \
  break; \
    case SIGUSR2: \
  memstate = Low; \
  break; \
    } \
} \
static void initMemalerter() \
{ \
    struct sigaction sa; \
    memset(&sa, '\0', sizeof sa); \
    sa.sa_handler = sig_handler; \
    sa.sa_flags = SA_RESTART; \
    if (sigaction(SIGHUP, &sa, NULL) < 0) { \
  return; \
    } \
    if (sigaction(SIGUSR1, &sa, NULL) < 0) { \
  return; \
    } \
    if (sigaction(SIGUSR2, &sa, NULL) < 0) { \
  return; \
    } \
    FILE *fo = fopen("/proc/sys/vm/freepg_signal_proc", "w"); \
     \
    if (!fo) \
        return; \
    fprintf(fo, "qpe\n"); \
    fclose(fo); \
}

#define QPE_INITIAL_NUMLOCK_STATE \
{ \
    bool numLock = FALSE; \
    sharp_kbdctl_modifstat  st; \
    int dev = ::open("/dev/sharp_kbdctl", O_RDWR); \
    if( dev >= 0 ) { \
  memset(&st, 0, sizeof(st)); \
  st.which = 3; \
                int ret = ioctl(dev, SHARP_KBDCTL_GETMODIFSTAT, (char*)&st); \
  if( !ret ) \
      numLock = (bool)st.stat; \
  ::close(dev); \
    } \
    return numLock; \
}
