/* 
 * Send string to GUI
 *
 * $Id: sendgui.cc,v 1.1 2002-11-12 22:12:33 mjm Exp $ 
 */

#include "config.hh"
#include "sendgui.hh"
#include "log.hh"

/* Simple dummy for alarm timer */
static void alarmdummy(int signo) 
{ 
  alarm (0);
}

/* Connect to given IP on given port */
int connect_server(int *sock, unsigned int ipaddr, int port) 
{
  struct sockaddr_in saddr;
  int retval=0;

  *sock = socket (PF_INET, SOCK_STREAM, 0);
  saddr.sin_family = PF_INET;
  saddr.sin_port = htons (port);
  saddr.sin_addr.s_addr = ipaddr;

  signal (SIGALRM, alarmdummy);
  siginterrupt (SIGALRM, 1);
  
  alarm(5);
  retval=connect (*sock, (struct sockaddr *) &saddr, sizeof (saddr));
  alarm(0);
  
  if(retval < 0)
  {
    close (*sock);
    return 0;
  }

 return 1;
}

/* Send a string to the GUI */
int sendgui(const char *string, ...) 
{
  int sock=0;
  char buffer[4096];
  va_list ap;

  /* Generate string */
  memset(buffer, 0, sizeof(buffer));
  va_start(ap, string);
  vsnprintf(buffer, sizeof(buffer)-1, string, ap);
  va_end(ap);

  if(!connect_server(&sock, inet_addr(GUIADDR), GUIPORT)) 
  {
    log_err("Connect to GUI at '%s' failed: %s", GUIADDR, strerror(errno));
    return 0;
  }

  if(write(sock, buffer, sizeof(buffer)) < 0)
  {
    log_err("Cannot write to socket: %s", strerror(errno));
    close(sock);
    return 0;
  }

  if(close(sock) < 0)
    log_err("Cannot close socket: %s", strerror(errno));

  return 1;
}

