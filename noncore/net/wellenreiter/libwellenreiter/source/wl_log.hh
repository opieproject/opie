/* $Id: wl_log.hh,v 1.1 2002-12-27 16:35:28 mjm Exp $ */

#ifndef WLLOG_HH
#define WLLOG_HH

#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <string.h>

void wl_loginfo(const char *, ...);
void wl_logerr(const char *, ...);

#endif /* WLLOG_HH */
