/* $Id: log.hh,v 1.1 2002-11-22 23:39:26 mjm Exp $ */

#ifndef WLLOG_HH
#define WLLOG_HH

#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <string.h>

void wl_loginfo(const char *, ...);
void wl_logerr(const char *, ...);

#endif /* WLLOG_HH */
