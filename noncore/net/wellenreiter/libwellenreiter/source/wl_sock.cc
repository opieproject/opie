/*
 * Socket operations for wellenreiter
 *
 * $Id: wl_sock.cc,v 1.4 2002-12-31 12:36:07 mjm Exp $
 */

#include "wl_sock.hh"
#include "wl_log.hh"

/* Setup UDP Socket for incoming commands */
int wl_setupsock(const char *host, int port, struct sockaddr_in saddr)
{
  int sock;

  if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
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
int wl_send(const char *host, int port, const char *string, ...) 
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

  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  saddr.sin_addr.s_addr = inet_addr(host);

  /* Setup socket */
  if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    wl_logerr("Cannot set up socket: %s", strerror(errno));
    return 0;
  }

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

/* Check for new messages on commsock */
int wl_recv(int *sock, struct sockaddr_in cliaddr, char *out, int maxlen)
{
  socklen_t len = sizeof(struct sockaddr);
  char retval[3];
 
  memset(out, 0, maxlen);
  if(recvfrom(*sock, out, maxlen - 1, 0, (struct sockaddr *)&cliaddr, &len) < 0)
  {
    wl_logerr("Cannot receive from socket: %s", strerror(errno));
    return -1;
  }

  /* Get packet type and return it */
  memset(retval, 0, sizeof(retval));
  memcpy(retval, out, 2);

  return atoi(retval);
}
