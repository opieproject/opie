/*
 * Startup functions of wellenreiter
 *
 * $Id: daemon.cc,v 1.5 2002-11-27 21:21:42 mjm Exp $
 */

#include "config.hh"
#include "daemon.hh"
#include "cardmode.hh"
#include "sniffer.hh"

/* Main function of wellenreiterd */
int main(int argc, char **argv)
{
  int sock, maxfd, retval;
  char buffer[128];
  pcap_t *handletopcap;             /* The handle to the libpcap */
  char errbuf[PCAP_ERRBUF_SIZE]; /* The errorbuffer of libpacap */
  struct pcap_pkthdr header;     /* The packet header from pcap*/
  const u_char *packet;          /* The actual packet content*/

  fd_set rset;

  fprintf(stderr, "wellenreiterd %s\n\n", VERSION);

#if 0
  /* will be replaced soon, just for max because max is lazy :-) */
  if(card_into_monitormode (SNIFFER_DEVICE, CARD_TYPE_NG) < 0)
  {
    fprintf(stderr, "Cannot set card into mon mode, aborting\n");
    exit(-1);
  }
#endif

  /* opening the pcap for sniffing */
  handletopcap = pcap_open_live(SNIFFER_DEVICE, BUFSIZ, 1, 1000, errbuf);
#ifdef HAVE_PCAP_NONBLOCK
  pcap_setnonblock(handletopcap, 1, errstr);
#endif

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
    //    FD_SET(pcap_fileno(handletopcap), &rset);
    //    maxfd=sock + pcap_fileno(handletopcap) + 1;
    maxfd=sock + 1;
    if(select(maxfd, &rset, NULL, NULL, NULL) < 0)
    {
      wl_logerr("Error calling select: %s", strerror(errno));
      break;
    }     

    /* Got data on local socket from GUI */
    if(FD_ISSET(sock, &rset))
    {

      if((retval=recvcomm(&sock, buffer, sizeof(buffer))) < 0)
      {
	wl_logerr("Error trying to read: %s", strerror(errno));
	break;
      }
      else
      {
	switch(retval)
	{
	  case 98:
	  {
	   wl_loginfo("Received STARTSNIFF command");
	   break;
	  };
	  case 99:
	  {
	      wl_loginfo("Received STOPSNIFF command");
	      break;
	  };
	  default:
	  {
	      wl_logerr("Received unknown command: %d", retval);
	      break;
	  };
	}
      } 
    } /* FD_ISSET */

#if 0
    /* Pcap stuff */
    if(FD_ISSET(pcap_fileno(handletopcap), &rset))
    {

      /* Grab one single packet */
      packet = pcap_next(handletopcap, &header);

      /* process the packet */
      process_packets(NULL,&header,*&packet);
    }
#endif
  }
  close(sock);
  exit(0);
}
