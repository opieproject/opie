/*
 * Small functions to log to syslog
 *
 * $Id: log.cc,v 1.1 2002-11-12 22:12:33 mjm Exp $
 */

#include "config.hh"
#include "log.hh"

/* Log to syslog INFO */
void log_info(const char *fmt,...) 
{

  char buffer[4096];
  va_list ap;

  memset(buffer, 0, sizeof(buffer)),
  va_start(ap, fmt);
  vsnprintf(buffer, sizeof(buffer)-1, fmt, ap);
  va_end(ap);
  
  openlog(PROGNAME, LOG_PID, LOG_SYSLOG);
  syslog(LOG_INFO, "(info) %s", buffer);
  closelog();

#ifdef DEBUG
  fprintf(stderr, "(info) %s\n", buffer);
#endif

}

/* Log to syslog ERR */
void log_err(const char *fmt,...) 
{

  char buffer[4096];
  va_list ap;

  memset(buffer, 0, sizeof(buffer));
  va_start(ap, fmt);
  vsnprintf(buffer, sizeof(buffer)-1, fmt, ap);
  va_end(ap);

  openlog(PROGNAME, LOG_PID, LOG_SYSLOG);
  syslog(LOG_INFO, "(err) %s", buffer); 	
  closelog();

#ifdef DEBUG
  fprintf(stderr, "(err) %s\n", buffer);
#endif        

}
