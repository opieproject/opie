/* 
 * Set card modes for sniffing
 *
 * $Id: cardmode.cc,v 1.22 2003-02-13 10:57:19 max Exp $
 */

#include "cardmode.hh"
#include "wl_log.hh"

/* main card into monitor function */
int card_into_monitormode (pcap_t **orighandle, const char *device, int cardtype)
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
      char wlanngcmd[80];
      snprintf(wlanngcmd, sizeof(wlanngcmd) - 1, "$(which wlanctl-ng) %s lnxreq_wlansniff channel=%d enable=true", device, 1);
      if (system(wlanngcmd) != 0)
      {
	  wl_logerr("Could not set %s in raw mode, check cardtype", device);
	  return 0;
      }
  }
  else if (cardtype == CARD_TYPE_HOSTAP)
  {
#if WIRELESS_EXT > 14
          // IW_MODE_MONITOR was implemented in Wireless Extensions Version 15
	  int skfd;
	  skfd = socket(AF_INET, SOCK_STREAM, 0);
	  struct iwreq wrq;
	  wrq.u.mode = IW_MODE_MONITOR;

	  if(iw_set_ext(skfd,(char *) device,SIOCSIWMODE,&wrq)<0)
 	 {
  	  wl_logerr("Could not set hostap card %s to raw mode, check cardtype", device);
  	  return 0;
      }
      else
      {
	    wl_loginfo("Successfully set hostap card %s into raw mode",device);
	    return 1;
      }
    return 1;
#else
       // Wireless Extensions < Version 15 need iwpriv commandos for monitoring
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
	   strcpy(ireq.ifr_ifrn.ifrn_name, device);
	   if (ioctl( fd, SIOCIWFIRSTPRIV + 4, &ireq)==0)
	   {
		  /* All was fine... */
		  close(fd);
  		wl_loginfo("Set hostap card %s into monitormode",device);
	       return 1;
	   }
	   else
	   {
	   	   /* iocall does not work */
			close(fd);
			wl_logerr("Could not set hostap card %s into monitormode, check cardtype",device);
			return 0;
	   }
#endif
  }
  else if (cardtype == CARD_TYPE_ORINOCCO )
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
int card_check_rfmon_datalink (const char *device)
{
  int datalinktype=0;
  pcap_t *phandle;
  phandle = pcap_open_live((char *)device, 65,0,0,NULL);
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
    err = ioctl(fd, SIOCGIFFLAGS, &ifr);
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
    if (cardtype == CARD_TYPE_CISCO || cardtype == CARD_TYPE_NG)
    {
    	/* Cisco and wlan-ng drivers don't need channelswitching */
    	return 1;
    }
    /* If it is a lucent orinocco card */ 
    else if (cardtype == CARD_TYPE_ORINOCCO)
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
	/* when it is an hostap card you need another iocall for channel switching */
	    else if (cardtype == CARD_TYPE_HOSTAP)
    {
	  int skfd;  	
      skfd = socket(AF_INET, SOCK_STREAM, 0);
	  struct iwreq wrq;
      iw_float2freq((double) channel, &wrq.u.freq);

	  if(iw_set_ext(skfd,(char *) device,SIOCSIWFREQ,&wrq)<0)
 	 {
  	  wl_logerr("Could not set hostap card %s to channel %d", device, channel);
  	  return 0;
      }
      else
      {
	    wl_loginfo("Successfully set hostap card %s to channel %d", device, channel);
	    return 1;
      }
	}
    /* For undefined situations */
	return 0;
}


int card_detect_channels (char * device)
{
	int skfd;
    skfd = socket(AF_INET, SOCK_STREAM, 0);
    struct iw_range	range;
    /* Get list of frequencies / channels */
		printf ("\n hostap card get the frequencies");
        /* Get list of frequencies / channels */
        if(iw_get_range_info(skfd, device, &range) < 0)
 	   {   /* We are failsave if something went wrong, asume only 11 channels */
 	   	return 11;
 	   }
 	   else
        {
        	if(range.num_frequency > 0)
			{
				return range.num_frequency;
			}
			else
			{   /* We are failsave asume only 11 channels */
				return 11;
			}
		}

} /* End of card_detect_channels */ 

int iw_get_range_info(int skfd, const char * ifname,  iw_range * range)
{
  struct iwreq wrq2;
  char   buffer[sizeof(iw_range) * 2];	/* Large enough */

  /* Cleanup */
  memset(buffer, 0, sizeof(buffer));
  wrq2.u.data.pointer = (caddr_t) buffer;
  wrq2.u.data.length = sizeof(buffer);
  wrq2.u.data.flags = 0;

  if(iw_get_ext(skfd, (char *)ifname, SIOCGIWRANGE, &wrq2) < 0)
  {
      wl_logerr("Could not get the range from the interface");
  	return(-1);
  }
  else
  {
      /* Copy stuff at the right place, ignore extra */
      memcpy((char *) range, buffer, sizeof(iw_range));
      return 0;
  }
}

/*------------------------------------------------------------------*/
/*
 * Convert our internal representation of frequencies to a floating point.
 */  
double iw_freq2float(iw_freq *  in)
{
  int           i;
  double        res = (double) in->m;
  for(i = 0; i < in->e; i++)
    res *= 10;
  return(res);
}


void iw_float2freq(double	in, iw_freq *	out)
{
  /* Version without libm : slower */
  out->e = 0;
  while(in > 1e9)
    {
      in /= 10;
      out->e++;
    }
  out->m = (long) in;
}



