/* 
 * Set card modes for sniffing
 *
 * $Id: cardmode.cc,v 1.2 2002-11-27 22:54:39 mjm Exp $
 */

#include "cardmode.hh"

/* main card into monitor function */
int card_into_monitormode (void *orighandle, char *device, int cardtype)
{
  int datalink; /* used for getting the pcap datalink type */
  char CiscoRFMON[35] = "/proc/driver/aironet/";
  FILE *CISCO_CONFIG_FILE;
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t *handle;
  
  handle = (pcap_t *)orighandle;

  /* Checks if we have a device to sniff on */
  if(device == NULL)
  {
      wl_logerr("No device given");
      return 0;
  }
  
  /* Setting the prmiscous and up flag to the interface */
  if (!card_set_promisc_up(device))
  {
    wl_logerr("Cannot set interface to promisc mode: %s", strerror(errno));
    return 0;
  } 
  wl_loginfo("Interface set to promisc mode");
  
  /* Check the cardtype and executes the commands to go into monitor mode */
  if (cardtype == CARD_TYPE_CISCO)
  {
      /* bring the sniffer into rfmon  mode */
      snprintf(CiscoRFMON, sizeof(CiscoRFMON), DEFAULT_PATH, device);
      if((CISCO_CONFIG_FILE = fopen(CiscoRFMON,"w")) == NULL)
      {
	  wl_logerr("Cannot open config file: %s", strerror(errno));
	  return 0;
      }
      fputs ("Mode: r",CISCO_CONFIG_FILE);
      fputs ("Mode: y",CISCO_CONFIG_FILE);
      fputs ("XmitPower: 1",CISCO_CONFIG_FILE);
      fclose(CISCO_CONFIG_FILE);
  }
  else if (cardtype == CARD_TYPE_NG)
  {
      char wlanngcmd[62];
      snprintf(wlanngcmd, sizeof(wlanngcmd), "%s %s lnxreq_wlansniff channel=1 enable=true", WLANCTL_PATH, device);
      if (system(wlanngcmd) != 0)
      {
	  wl_logerr("Could not set %s in raw mode, check cardtype", device);
	  return 0;
      }
  }
  else if (cardtype == CARD_TYPE_HOSTAP)
  {
      wl_logerr("Got a host-ap card, nothing is implemented now");
  }
   
  /* Check the interface if it is in the correct raw mode */
  if((handle = pcap_open_live(device, BUFSIZ, 1, 0, errbuf)) == NULL)
  {
      wl_logerr("pcap_open_live() failed: %s", strerror(errno));
      return 0;
  }
  
#ifdef HAVE_PCAP_NONBLOCK
  pcap_setnonblock(handle, 1, errstr);
#endif
  
  /* getting the datalink type */
  datalink = pcap_datalink(handle);
  
  if (datalink != DLT_IEEE802_11) /* Rawmode is IEEE802_11 */
  {
      wl_loginfo("Interface %s does not work in the correct 802.11 raw mode", device);
      pcap_close(handle); 
      return 0;
  }
  wl_loginfo("Your successfully listen on %s in 802.11 raw mode", device);
  
  return 1;
}

/* Set card into promisc mode */
int card_set_promisc_up (const char *device)
{
  char ifconfigcmd[32];
  int retval=0;

  snprintf(ifconfigcmd, sizeof(ifconfigcmd), SBIN_PATH, device);
  retval = system(ifconfigcmd);

  if(retval < 0 || retval == 0)
    return 0;
  
return 1;
}
