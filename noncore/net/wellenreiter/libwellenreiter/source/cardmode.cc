/* 
 * Set card modes for sniffing
 *
 * $Id: cardmode.cc,v 1.15 2003-02-09 21:00:55 max Exp $
 */

#include "cardmode.hh"
#include "wl_log.hh"

/* main card into monitor function */
int card_into_monitormode (pcap_t **orighandle, char *device, int cardtype)
{
  char CiscoRFMON[35] = "/proc/driver/aironet/";
  FILE *CISCO_CONFIG_FILE;

  /* Checks if we have a device to sniff on */
  if(device == NULL)
  {
      wl_logerr("No device given");
      return 0;
  }
  
  /* Setting the promiscous and up flag to the interface */
  if (!card_set_promisc_up(device))
  {
    wl_logerr("Cannot set interface to promisc mode");
    return 0;
  } 
  wl_loginfo("Interface set to promisc mode");
  
  /* Check the cardtype and executes the commands to go into monitor mode */
  if (cardtype == CARD_TYPE_CISCO)
  {
      /* bring the sniffer into rfmon  mode */
      snprintf(CiscoRFMON, sizeof(CiscoRFMON) - 1, DEFAULT_PATH, device);
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
      snprintf(wlanngcmd, sizeof(wlanngcmd) - 1, "$(which wlanctl-ng) %s lnxreq_wlansniff channel=%d enable=true", device, 1);
      if (system(wlanngcmd) != 0)
      {
	  wl_logerr("Could not set %s in raw mode, check cardtype", device);
	  return 0;
      }
  }
  else if (cardtype == CARD_TYPE_HOSTAP)
  {
      wl_logerr("Got a host-ap card, nothing is implemented now");
     char hostapcmd[250];
     snprintf(hostapcmd, sizeof(hostapcmd) -1, "$(which iwpriv) %s monitor 2 %d", device,1);
     if (system(hostapcmd) !=0)
     {
 	wl_logerr("Could not set %s in raw mode, check cardtype", device);
	return 0;
     }
  }
  else if (cardtype == CARD_TYPE_ORINOCCO || cardtype == CARD_TYPE_HOSTAP)
  {
	 if (!card_set_channel (device, 1, CARD_TYPE_ORINOCCO))
	 {
  	  wl_logerr("Could not set %s in raw mode, check cardtype", device);
  	  return 0;
      }
      else
      {
	    wl_loginfo("Successfully set %s into raw mode",device);
      }
  }

  /* Setting the promiscous and up flag to the interface */
  if (!card_check_rfmon_datalink(device))
  {
    wl_logerr("Cannot set interface to rfmon mode");
    return 0;
  }
  else 
  {
    wl_loginfo("Interface set to rfmon mode");
  }
  return 1;
}

/* Check card is in the rfmon mode */
int card_check_rfmon_datalink (char *device)
{
  int datalinktype=0;
  pcap_t *phandle;
  phandle = pcap_open_live(device, 65,0,0,NULL);
  datalinktype = pcap_datalink (phandle);
  pcap_close(phandle);

  if (datalinktype != DLT_IEEE802_11) /* Rawmode is IEEE802_11 */
    {
        return 0;
    }
  else
  {
    wl_loginfo("Your successfully listen on %s in 802.11 raw mode", device);
    return 1; 
  }
}

/* Set card into promisc mode */
int card_set_promisc_up (const char *device)
{
    int err;
    /* First generate a socket to use with iocalls */
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        /* In case of an error */
        perror("socket");
        return 0;
    }

    /* Fill an empty an interface structure with the right flags (UP and Promsic) */
    struct ifreq ifr;
    strncpy(ifr.ifr_name, device,10);
    ifr.ifr_flags = IFF_UP + IFF_PROMISC;
    err = ioctl(fd, SIOCSIFFLAGS, &ifr);
    if (err < 0)
    {
    	perror("Could not access the interface, ");
    	close(fd);
        return 0;
    }
    
    /* Get the informations back from the interface to check if the flags are correct */
    strncpy(ifr.ifr_name, device,10);
    ioctl(fd, SIOCGIFFLAGS, &ifr);
    if (err < 0)
    {
        perror("Could not access the interface, ");
    	close(fd);
        return 0;
    }

    if(ifr.ifr_flags && IFF_UP)
    {
    	close(fd);
        return 1;
    }
    else
    {
		wl_logerr("Could not set promisc flag on %d", device);
		close(fd);
        return 0;
    }
}

/* Set channel (Wireless frequency) of the device */
int card_set_channel (const char *device, int channel, int cardtype)
{
    if (cardtype == CARD_TYPE_CISCO)
    {
    	/* Cisco cards don't need channelswitching */
    	return 1;
    }
    /* If it is a lucent orinocco card */ 
    else if (cardtype == CARD_TYPE_ORINOCCO || cardtype == CARD_TYPE_HOSTAP)
    {
    	int fd;
   	//Wireless tools structure for the iocalls
   	struct iwreq ireq;  
   	int *ptr;
	   /* Socket needed to use the iocall to */
	   fd = socket(AF_INET, SOCK_STREAM, 0);

	   if ( fd == -1 ) {
 	     return -1;
 	  }
	   ptr = (int *) ireq.u.name;
	   // This is the monitor mode for 802.11 non-prism header
	   ptr[0] = 2;
	   ptr[1] = channel;
	   strcpy(ireq.ifr_ifrn.ifrn_name, device);
	   if (ioctl( fd, SIOCIWFIRSTPRIV + 0x8, &ireq)==0)
	   {
		  /* All was fine... */
		  close(fd);
  		wl_loginfo("Set channel %d on interface %s",channel, device);
	       return 1;
	   }
	   else
	   {
	   	   /* iocall does not work */
			close(fd);
			wl_logerr("Could not set channel %d on %s, check cardtype",channel, device);
			return 0;
	   }
    }
    else if (cardtype == CARD_TYPE_NG)
	{
		char wlanngcmd[62];
		snprintf(wlanngcmd, sizeof(wlanngcmd) - 1, "$(which wlanctl-ng) %s lnxreq_wlansniff channel=%d enable=true", device, channel);
		if (system(wlanngcmd) != 0)
		{
			wl_logerr("Could not set channel %d on %s, check cardtype",channel, device);
			return 0;
		}

	}
    /* For undefined situations */
	return 0;
}

