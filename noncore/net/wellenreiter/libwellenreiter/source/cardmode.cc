/* 
 * Set card modes for sniffing
 *
 * $Id: cardmode.cc,v 1.10 2003-02-07 03:23:16 max Exp $
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
      snprintf(wlanngcmd, sizeof(wlanngcmd) - 1, "%s %s lnxreq_wlansniff channel=1 enable=true", WLANCTL_PATH, device);
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
     snprintf(hostapcmd, sizeof(hostapcmd) -1, "%s %s monitor 2", IWPRIV_PATH, device);
     if (system(hostapcmd) !=0)
     {
 	wl_logerr("Could not set %s in raw mode, check cardtype", device);
	return 0;
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
        return 0;
    }
    
    /* Get the informations back from the interface to check if the flags are correct */
    strncpy(ifr.ifr_name, device,10);
    ioctl(fd, SIOCGIFFLAGS, &ifr);
    if (err < 0)
    {
        perror("Could not access the interface, ");
        return 0;
    }

    if(ifr.ifr_flags && IFF_UP)
    {
     printf("%s is ok\n", device);
     return 1;
    }
    else
    {
     printf("%s flags could not be set", device);
     return 0;
    }
}
