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

#define QPE_NEED_CALIBRATION
#define QPE_HAVE_TOGGLELIGHT
#define QPE_STARTMENU
#define USE_REALTIME_AUDIO_THREAD
#define OPIE_NEW_MALLOC


#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>

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
