/*
 * Socket operations for wellenreiter
 *
 * $Id: sock.cc,v 1.1 2002-11-22 23:39:26 mjm Exp $
 */

#include "sock.hh"
#include "log.hh"

/* Setup UDP Socket for incoming commands */
int commsock(const char *host, int port)
{
  struct sockaddr_in saddr;
  int sock;

  if((sock=socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    wl_logerr("Cannot set up socket: %s", strerror(errno));
    return -1;
  }

  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = PF_INET;
  saddr.sin_port = htons(port);
  saddr.sin_addr.s_addr = inet_addr(host);
 
  if(bind(sock,(struct sockaddr *)&saddr, sizeof(saddr)) < 0)
  {
    wl_logerr("Cannot bind socket: %s", strerror(errno));
    close(sock);
    return -1;
  }

  return sock;
}

/* Send a string to commsock */
int sendcomm(const char *host, int port, const char *string, ...) 
{
  int sock;
  char buffer[4096];
  struct sockaddr_in saddr;
  va_list ap;

  /* Generate string */
  memset(buffer, 0, sizeof(buffer));
  va_start(ap, string);
  vsnprintf(buffer, sizeof(buffer)-1, string, ap);
  va_end(ap);

  /* Setup socket */
  if((sock=socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    wl_logerr("Cannot set up socket: %s", strerror(errno));
    return -1;
  }

  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  saddr.sin_addr.s_addr = inet_addr(host);

  if(sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
  {
    wl_logerr("Cannot write to socket: %s", strerror(errno));
    close(sock);
    return 0;
  }

  if(close(sock) < 0)
    wl_logerr("Cannot close socket: %s", strerror(errno));

  return 1;
}
