/*
 * Startup functions of wellenreiter
 *
 * $Id: daemon.cc,v 1.1 2002-11-12 22:12:33 mjm Exp $
 */

#include "config.hh"
#include "daemon.hh"
#include "log.hh"
#include "sendgui.hh"
#include "getgui.hh"

/* Main function of wellenreiterd */
int main(int argc, char **argv)
{
  int sock, maxfd;
  struct sockaddr_in *cliaddr;
  socklen_t len=sizeof(struct sockaddr);
  char buffer[128];
  FILE *fp=stdin; /* Will be replaced with sniffer */
  fd_set rset;

  fprintf(stderr, "wellenreiterd %s\n\n", VERSION);

  /* Setup socket for incoming commands */
  if(!commsock(&sock))
    return 0;

  log_info("Set up socket '%d' for GUI communication", sock);

  FD_ZERO(&rset);

  /* Start main loop */
  log_info("Starting main loop");
  while(1)
  {

    FD_SET(sock, &rset);
    FD_SET(fileno(fp), &rset);
    maxfd=sock+fileno(fp)+1;
    if(select(maxfd, &rset, NULL, NULL, NULL) < 0)
    {
      log_err("Error calling select: %s", strerror(errno));
      break;
    }     

    /* Got data on local socket from GUI */
    if(FD_ISSET(sock, &rset))
    {
      memset(buffer, 0, sizeof(buffer));
      if(recvfrom(sock, buffer, sizeof(buffer)-1, 0, (struct sockaddr *)cliaddr, &len) < 0)
      {
	 log_err("Cannot read from socket: %s", strerror(errno));
	 break;
      }
      log_info("Received command from '%s': %s", inet_ntoa(cliaddr->sin_addr), buffer); 

      /* Pass string to analyze function */
      commstring(buffer);

    }

    /* Will be replaced with sniffer ... later */
    if(FD_ISSET(fileno(fp), &rset))
    {
      memset(buffer, 0, sizeof(buffer));
      if(fgets(buffer, sizeof(buffer) - 1, fp) == NULL)
      {
	 log_err("Cannot read from stdin: %s", strerror(errno));
	 break;
      }

      /* Send string to GUI */
      sendgui("%d: %s", 1234, buffer);

    }

  }

  close(sock);
  return 0;
}
