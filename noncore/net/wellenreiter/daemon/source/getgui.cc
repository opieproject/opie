/*
 * Setup UDP socket for commands
 * Misc wrapper functions for incoming commands
 *
 * $Id: getgui.cc,v 1.1 2002-11-12 22:12:33 mjm Exp $
 */

#include "config.hh"
#include "getgui.hh"
#include "log.hh"

struct sockaddr_in saddr;

/* Setup UDP Socket for incoming commands */
int commsock(int *sock)
{

  if((*sock=socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    log_err("Cannot set up socket: %s", strerror(errno));
    return 0;
  }

  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = PF_INET;
  saddr.sin_port = htons(DAEMONPORT);
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
  if(bind(*sock,(struct sockaddr *)&saddr, sizeof(saddr)) < 0)
  {
    log_err("Cannot bind socket: %s", strerror(errno));
    close(*sock);
    return 0;
  }

  return 1;
}

int commstring(const char *input)
{

  return 1;
}
