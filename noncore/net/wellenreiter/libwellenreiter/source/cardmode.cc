/* 
 * Set card modes for sniffing
 *
 * $Id: cardmode.cc,v 1.9 2003-01-31 10:42:13 max Exp $
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
    wl_logerr("Cannot set interface to promisc mode: %s", strerror(errno));
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
   
  return 1;
}

/* Set card into promisc mode */
int card_set_promisc_up (const char *device)
{
    struct ifconf ifc;
    struct ifreq ifr_x[50];
    u_int i;
    int sockfd, err;
	err=0;
	/* opening a socket for issuing the iocalls */
	sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
 
    if (sockfd < 0) 
    {
    	/* In case of an error  - mjm proove that please*/
        perror("socket");
		return 1;
    }
	/* Define room for 50 interfaces */
    ifc.ifc_len = 50 * sizeof(struct ifreq);
    ifc.ifc_req = ifr_x;
	/* Get the config of the interfaces */
    err = ioctl(sockfd, SIOCGIFCONF, &ifc);
    if (err == -1) return 1;       
    
    /* For each interface*/
    for (i = 0; i < ifc.ifc_len / sizeof(struct ifreq); i++) 
	{
		/* To complete , should get the IP, if no is assigned, asign one */
		/*err = ioctl(sockfd, SIOCGIFADDR, &ifr_x[i]);
		if (err == -1) perror("SIOCGIFADDR: ");		
		printf ("Address: %s\n",Sock_ntop_host(ifr_x[i].ifr_addr.sa_family,sizeof(ifr_x[i].ifr_addr.sa_family)));
		*/
		if(strncmp(ifr_x[i].ifr_name,device,5) == 0)
		{
			/* Get the flags */
			err = ioctl(sockfd, SIOCGIFFLAGS, &ifr_x[i]);
        	if (err == -1) 
        	{
        		perror("SIOCGIFFLAGS: ");
        		return 1;
        	}
			/* Check if the Interface is UP and PROMISC */
			if (ifr_x[i].ifr_flags & IFF_PROMISC && ifr_x[i].ifr_flags & IFF_UP)
			{
				/* only debug text */
				printf ("%s is PROMISC and UP \n",ifr_x[i].ifr_name);
			}
			else
			{
				/* Set the promisc flag to the interface */
				ifr_x[i].ifr_flags=ifr_x[i].ifr_flags+IFF_PROMISC;
				err = ioctl(sockfd, SIOCSIFFLAGS, &ifr_x[i]);
				if (err == -1) 
				{
					/* Could not set the interface into promisc mode */
					perror("SIOCSIFFLAGS: ");
				}
			}
		}
      }
        /* All is fine */
        return 0;
}
