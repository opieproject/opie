/* -*- C++ -*-
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: kpppconfig.h,v 1.1 2003-05-22 15:08:21 tille Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#ifndef _KPPPCONFIG_H_
#define _KPPPCONFIG_H_

#if defined(__svr4__)
#define STREAMS
#define _XOPEN_SOURCE 1
#define _XOPEN_SOURCE_EXTENDED 1
#define __EXTENSIONS__
#endif

//#include <config.h>

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

// Warning: If you fiddle with the following directories you have
// to adjust make_directories() in main.cpp()

// file used for PAP authetication purposes
#define PAP_AUTH_FILE "/etc/ppp/pap-secrets"

// file used for CHAP authetication purposes
#define CHAP_AUTH_FILE "/etc/ppp/chap-secrets"

// Define the default modem response timeout
#define MODEM_TIMEOUT 60

// Define the default modem tone duration (ATS11=)
#define MODEM_TONEDURATION 70

// Define the default time for pppd to get the interface up
#define PPPD_TIMEOUT 30

// Define the default time to wait after a busy signal before redialing
#define BUSY_WAIT 0



// Every PPP_STATS_INTERVAL milli seconds kppp will read
// and display the ppp statistics IF the stats window
// is visible. If the stats window is not visible
// the stats are not taken.
// 200 milli secs is 5 times per second and results in
// 0 load on my machine. Play with this parameter if
// you feel like it.

#define PPP_STATS_INTERVAL 200
// comment this out to get some more debugging info
/*
#define MY_DEBUG
*/

// Define the maximum number of accounts
#define MAX_ACCOUNTS 100

// Define the mamimum number of script entries
#define MAX_SCRIPT_ENTRIES 20

// Define the maximun number of DNS entries
#define MAX_DNS_ENTRIES 5

// Maximum size of the command executing pppd
const unsigned int MAX_CMDLEN = 2024;

// Define the maximum number of arguments passed to the pppd daemon
#define MAX_PPPD_ARGUMENTS 20

// Define the maximun number of lines of /etc/resolv.conf
#define MAX_RESOLVCONF_LINES 128

// Directory for modem lock files (Needed by mgetty users)
#ifdef __linux__
# define LOCK_DIR "/var/lock"
#else /* linux */
# ifdef BSD
#  define	LOCK_DIR "/var/spool/lock"
# else  /* BSD */
#  define	LOCK_DIR "/var/spool/locks"
# endif /* BSD */
#endif  /* linux */

#define DEVNULL "/dev/null"

// search path for pppd binary
#define PPPDSEARCHPATH "/sbin:/usr/sbin:/usr/local/sbin:/usr/bin:/usr/local/bin"

// name of the pppd binary
#define PPPDNAME "pppd"

// support for internal ISDN cards and modem emulation
#ifdef __linux__
#define ISDNSUPPORT
#endif

#if defined(__linux__) || defined(BSD)
#define PPP_PID_DIR "/var/run/"
#else
#define PPP_PID_DIR "/etc/ppp/"
#endif

// defined in opener.cpp
extern const char * const kppp_syslog[];

#ifdef _XPG4_2
#define __xnet_connect connect
#endif

#endif
