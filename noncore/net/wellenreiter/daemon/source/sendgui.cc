/* 
 * Send string to GUI
 *
 * $Id: sendgui.cc,v 1.2 2002-11-13 21:57:19 mjm Exp $ 
 */

#include "config.hh"
#include "sendgui.hh"
#include "log.hh"

/* Send a string to the GUI */
int sendgui(const char *string, ...) 
{
  int sock=0;
  char buffer[4096];
  struct sockaddr_in saddr;
  va_list ap;

  /* Generate string */
  memset(buffer, 0, sizeof(buffer));
  va_start(ap, string);
  vsnprintf(buffer, sizeof(buffer)-1, string, ap);
  va_end(ap);

  /* Setup socket */
  sock = socket (AF_INET, SOCK_DGRAM, 0);
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(GUIPORT);
  saddr.sin_addr.s_addr = inet_addr(GUIADDR);

  if(sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
  {
    log_err("Cannot write to socket: %s", strerror(errno));
    close(sock);
    return 0;
  }

  if(close(sock) < 0)
    log_err("Cannot close socket: %s", strerror(errno));

  return 1;
}
