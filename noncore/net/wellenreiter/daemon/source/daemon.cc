/*
 * Startup functions of wellenreiter
 *
 * $Id: daemon.cc,v 1.3 2002-11-22 23:41:17 mjm Exp $
 */

#include "config.hh"
#include "daemon.hh"

/* Main function of wellenreiterd */
int main(int argc, char **argv)
{
  int sock, maxfd, guiport=GUIPORT;
  char guihost[]="127.0.0.1";
  struct sockaddr_in *cliaddr;
  socklen_t len=sizeof(struct sockaddr);
  char buffer[128];
  FILE *fp=stdin; /* Will be replaced with sniffer */
  fd_set rset;

  fprintf(stderr, "wellenreiterd %s\n\n", VERSION);

  /* Setup socket for incoming commands */
  if((sock=commsock(DAEMONADDR, DAEMONPORT)) < 0)
  {
    wl_logerr("Cannot setup socket");
    exit(-1);
  }
  wl_loginfo("Set up socket '%d' for GUI communication", sock);

  FD_ZERO(&rset);

  /* Start main loop */
  wl_loginfo("Starting main loop");
  while(1)
  {

    FD_SET(sock, &rset);
    FD_SET(fileno(fp), &rset);
    maxfd=sock+fileno(fp)+1;
    if(select(maxfd, &rset, NULL, NULL, NULL) < 0)
    {
      wl_logerr("Error calling select: %s", strerror(errno));
      break;
    }     

    /* Got data on local socket from GUI */
    if(FD_ISSET(sock, &rset))
    {
      memset(buffer, 0, sizeof(buffer));
      if(recvfrom(sock, buffer, sizeof(buffer)-1, 0, (struct sockaddr *)cliaddr, &len) < 0)
      {
	 wl_logerr("Cannot read from socket: %s", strerror(errno));
	 break;
      }
      wl_loginfo("Received command from '%s': %s", inet_ntoa(cliaddr->sin_addr), buffer); 

      /* Pass string to analyze function */
      //      sendcomm(guihost, guiport, buffer);

    }

    /* Will be replaced with sniffer ... later */
    if(FD_ISSET(fileno(fp), &rset))
    {
      memset(buffer, 0, sizeof(buffer));
      if(fgets(buffer, sizeof(buffer) - 1, fp) == NULL)
      {
	 wl_logerr("Cannot read from stdin: %s", strerror(errno));
	 break;
      }
      wl_loginfo("Sending command to '%s': %s", GUIADDR, buffer);

      /* Send string to GUI */
      sendcomm(guihost, guiport, "%d: %s", 1234, buffer);

    }
  }
  close(sock);
  exit(0);
}
