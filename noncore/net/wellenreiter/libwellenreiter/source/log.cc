/*
 * Small functions to log to syslog
 *
 * $Id: log.cc,v 1.1 2002-11-22 23:39:26 mjm Exp $
 */

#include "log.hh"

/* Log to syslog INFO */
void wl_loginfo(const char *fmt,...) 
{

  char buffer[4096];
  va_list ap;

  memset(buffer, 0, sizeof(buffer));
  va_start(ap, fmt);
  vsnprintf(buffer, sizeof(buffer)-1, fmt, ap);
  va_end(ap);
  
  openlog("libwellenreiter", LOG_PID, LOG_SYSLOG);
  syslog(LOG_INFO, "(info) %s", buffer);
  closelog();

#ifdef DEBUG
  fprintf(stderr, "(info) %s\n", buffer);
#endif

}

/* Log to syslog ERR */
void wl_logerr(const char *fmt,...) 
{

  char buffer[4096];
  va_list ap;

  memset(buffer, 0, sizeof(buffer));
  va_start(ap, fmt);
  vsnprintf(buffer, sizeof(buffer)-1, fmt, ap);
  va_end(ap);

  openlog("libwellenreiter", LOG_PID, LOG_SYSLOG);
  syslog(LOG_INFO, "(err) %s", buffer); 	
  closelog();

#ifdef DEBUG
  fprintf(stderr, "(err) %s\n", buffer);
#endif        

}
