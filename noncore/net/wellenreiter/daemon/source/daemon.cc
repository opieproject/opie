/*
 * Startup functions of wellenreiter
 *
 * $Id: daemon.cc,v 1.10 2002-12-11 14:57:54 mickeyl Exp $
 */

#include "config.hh"
#include "daemon.hh"

/* Main function of wellenreiterd */
int main(int argc, char **argv)
{
  int sock, maxfd, retval;
  char buffer[128];
  struct pcap_pkthdr header;
  pcap_t *handletopcap;
  const unsigned char *packet;

  fd_set rset;

  fprintf(stderr, "wellenreiterd %s\n\n", VERSION);

  if(!card_into_monitormode(&handletopcap, SNIFFER_DEVICE, CARD_TYPE))
  {
    wl_logerr("Cannot set card into mon mode, aborting");
    exit(-1);
  }
  wl_loginfo("Set card into monitor mode");

  /////// following line will be moved to lib as soon as possible ////////////
    if((handletopcap = pcap_open_live(SNIFFER_DEVICE, BUFSIZ, 1, 0, NULL)) == NULL)
    {
      wl_logerr("pcap_open_live() failed: %s", strerror(errno));
      exit(-1);
    }

#ifdef HAVE_PCAP_NONBLOCK
    pcap_setnonblock(handletopcap, 1, NULL);
#endif
      
    /* getting the datalink type */
    retval = pcap_datalink(handletopcap);
    if (retval != DLT_IEEE802_11) /* Rawmode is IEEE802_11 */
      {
	wl_loginfo("Interface %s does not work in the correct 802.11 raw mode", SNIFFER_DEVICE);
	pcap_close(handletopcap);
	return 0;
      }
    wl_loginfo("Your successfully listen on %s in 802.11 raw mode", SNIFFER_DEVICE);
    ////////////////////////////////////////
 
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
    FD_SET(pcap_fileno(handletopcap), &rset);
    maxfd=sock + pcap_fileno(handletopcap) + 1;

    if(select(maxfd, &rset, NULL, NULL, NULL) < 0)
    {
      wl_logerr("Error calling select: %s", strerror(errno));
      break;
    }     

    /* Got data on local socket from GUI */
    if(FD_ISSET(sock, &rset))
    {
      /* Receive data from socket */
      if((retval=recvcomm(&sock, buffer, sizeof(buffer))) < 0)
      {
	wl_logerr("Error trying to read: %s", strerror(errno));
	break;
      }
      else
      {
	/* check type of packet and start function according to it */
	switch(retval)
	{
	  case 98:
	   wl_loginfo("Received STARTSNIFF command");
	   break;
	  case 99:
	      wl_loginfo("Received STOPSNIFF command");
	      break;
	  default:
	      wl_logerr("Received unknown command: %d", retval);
	      break;
	}
      } 
    } /* FD_ISSET */

    /* Check pcap lib for packets */
    if(FD_ISSET(pcap_fileno(handletopcap), &rset))
    {

      /* Grab one single packet */
      packet = pcap_next(handletopcap, &header);

      /* process the packet */
      process_packets(&header,*&packet, GUIADDR, GUIPORT);
    }

  } /* while(1) */

  close(sock);
  exit(0);
}
