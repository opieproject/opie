/*
 * Startup functions of wellenreiter
 *
 * $Id: daemon.cc,v 1.25 2003-03-04 14:04:52 max Exp $
 */

#include "config.hh"
#include "daemon.hh"

/* should be parsed from cfg-file */
#define MAXCHANNEL 13
#define CHANINTERVAL 500000


/* Main function of wellenreiterd */
int main(int argc, char **argv)
{
  int sock, maxfd, retval;
  char buffer[WL_SOCKBUF];
  struct pcap_pkthdr header;
  struct sockaddr_in saddr;
//  pcap_t *handletopcap;
  wl_cardtype_t cardtype;
  pthread_t sub;
  const unsigned char *packet;

  fd_set rset;

  fprintf(stderr, "wellenreiterd %s\n\n", VERSION);
  fprintf(stderr, "(c) 2002 by M-M-M\n\n");

  if(argc < 3)
    usage();
  
  /* Set sniffer device */
  memset(cardtype.iface, 0, sizeof(cardtype.iface));
  strncpy(cardtype.iface, (char *)argv[1], sizeof(cardtype.iface) - 1);

  /* Set card type */
  cardtype.type = atoi(argv[2]);
  if(cardtype.type < 1 || cardtype.type > 4)
    usage();

  /* Until we do not act as a read daemon, it starts the sniffer 
     right after startup */
  if (!start_sniffer(cardtype.iface,cardtype.type))
  {
  	wl_logerr("daemon, start_sniff did not return proper, aborting");
  	exit(EXIT_FAILURE);
  }
  wl_loginfo ("daemon, wireless card prepared for sniffing");
  
  /* Setup socket for incoming commands */
  if((sock=wl_setupsock(DAEMONADDR, DAEMONPORT, saddr)) < 0)
  {
    wl_logerr("Cannot setup socket");
    exit(EXIT_FAILURE);
  } 
  wl_loginfo("Set up socket '%d' for GUI communication", sock);

  /* Create channelswitching thread */
  if(pthread_create(&sub, NULL, channel_switcher, 
		    (void *)&cardtype) != 0)
  {
    wl_logerr("Cannot create thread: %s", strerror(errno));
    close(sock);
    exit(EXIT_FAILURE);
  } 
  if(pthread_detach(sub))
  {
    wl_logerr("Error detaching thread");
    close(sock);
    pthread_exit((pthread_t *)sub);
    exit(EXIT_FAILURE);
  }
  wl_loginfo("Created and detached channel switching thread");
  
  FD_ZERO(&rset);

  /* Start main loop */
  wl_loginfo("Starting main loop");
  while(1)
  {

    FD_SET(sock, &rset);
    FD_SET(pcap_fileno(handletopcap), &rset);

    /* maxfd = biggest filefd */
    maxfd = (sock > pcap_fileno(handletopcap) ? 
	     sock + 1 : pcap_fileno(handletopcap)) + 1;

    if(select(maxfd, &rset, NULL, NULL, NULL) < 0)
    {
      wl_logerr("Error calling select: %s", strerror(errno));
      break;
    }     

    /* Got data on local socket from GUI */
    if(FD_ISSET(sock, &rset))
    {
      /* Receive data from socket */
      if((retval=wl_recv(&sock, saddr, buffer, sizeof(buffer))) < 0)
      {
	wl_logerr("Error trying to read: %s", strerror(errno));
	break;
      }
      else
      {
	/* check type of packet and start function according to it */
	switch(retval)
	{
	case STARTSNIFF:
	  wl_loginfo("Received STARTSNIFF command");
	  if(!send_ok(GUIADDR, GUIPORT, STARTSNIFF))
	    wl_logerr("Cannot set OK_CMD to GUI");
	  break;
	case STOPSNIFF:
	  wl_loginfo("Received STOPSNIFF command");
	  if(!send_ok(GUIADDR, GUIPORT, STOPSNIFF))
	    wl_logerr("Cannot set FAIL_CMD to GUI");
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
  exit(EXIT_SUCCESS);
}

void 
usage(void)
{
  fprintf(stderr, "Usage: wellenreiter <device> <cardtype>\n"      \
	  "\t<device>   = Wirelessdevice (e.g. wlan0)\n" \
	  "\t<cardtype> = Cardtype:\tCisco\t= 1\n"       \
	  "\t\t\t\tNG\t= 2\n"                             \
          "\t\t\t\tHOSTAP\t= 3\n"			  \
	  "\t\t\t\tLUCENT\t= 4\n");
  exit(EXIT_FAILURE);
}

void * 
channel_switcher(void *cardtypeptr)
{
  wl_cardtype_t *cardtype;
  int maxchan=0;
  int channel=1;
  /* Get card info struct */
  cardtype = (wl_cardtype_t *)cardtypeptr;
  /* Get from the wireless extension the amount of available channels
     this prevents a fail in switching on us cards */
  maxchan = card_detect_channels(cardtype->iface);
  
  if (maxchan < MAXCHANNEL)
  {
  	#undef MAXCHANNEL
  	#define MAXCHANNEL maxchan
  }
  
  while(1)
  {

    /* If channel bigger than maxchannel, set to 1 */
    if(channel > MAXCHANNEL)
      channel=1;
    
    /* Set channel */
    if(!card_set_channel(cardtype->iface, channel, cardtype->type))
    {
      wl_logerr("Cannot set channel, thread exiting");
      pthread_exit(NULL);
    }
    
    /* sleep */
    usleep(CHANINTERVAL);
    
    channel++;
  } /* while */
}
