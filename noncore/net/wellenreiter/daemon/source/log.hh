/* $Id: log.hh,v 1.1 2002-11-12 22:12:33 mjm Exp $ */

#ifndef LOG_HH
#define LOG_HH

#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <string.h>

void log_info(const char *, ...);
void log_err(const char *, ...);

#endif /* LOG_HH */
