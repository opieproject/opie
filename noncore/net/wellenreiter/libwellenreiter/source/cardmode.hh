/* $Id: cardmode.hh,v 1.17 2003-02-12 01:03:29 max Exp $ */

#ifndef CARDMODE_HH
#define CARDMODE_HH

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/if.h>
#include <linux/wireless.h>

#ifndef SIOCIWFIRSTPRIV
#define SIOCIWFIRSTPRIV SIOCDEVPRIVATE
#endif


extern "C"
{
#include <net/bpf.h>
#include <pcap.h>  
}

/* Defines, used for the card setup */
#define   DEFAULT_PATH       "/proc/driver/aironet/%s/Config"
#define   CISCO_STATUS       "/proc/driver/aironet/%s/Status"
#define   CARD_TYPE_CISCO	 1
#define   CARD_TYPE_NG	    2
#define   CARD_TYPE_HOSTAP	3
#define   CARD_TYPE_ORINOCCO  4

/* Some usefull constants for frequencies */
#define KILO    1e3
#define MEGA    1e6
#define GIGA    1e9


/* only for now, until we have the daemon running */
/*the config file should provide these information */
#define   CARD_TYPE              CARD_TYPE_HOSTAP

/* Prototypes */
int card_check_rfmon_datalink (const char *device);
int card_into_monitormode (pcap_t **, const char *, int);
int card_set_promisc_up (const char *);
int card_set_channel (const char *device, int channel,int cardtype); 
int iw_get_range_info(int skfd, const char * ifname,  struct iw_range * range);
double iw_freq2float(iw_freq * in);
void iw_float2freq(double in, iw_freq *	out);
int card_detect_channels (char * device);


/*------------------------------------------------------------------*/
/*
 * Wrapper to push some Wireless Parameter in the driver
 */
static inline int
iw_set_ext(int			skfd,		/* Socket to the kernel */
	   char *		ifname,		/* Device name */
	   int			request,	/* WE ID */
	   struct iwreq *	pwrq)		/* Fixed part of the request */
{
  /* Set device name */
  strncpy(pwrq->ifr_name, ifname, IFNAMSIZ);
  /* Do the request */
  return(ioctl(skfd, request, pwrq));
}

/*------------------------------------------------------------------*/
/*
 * Wrapper to extract some Wireless Parameter out of the driver
 */
static inline int
iw_get_ext(int			skfd,		/* Socket to the kernel */
	   char *		ifname,		/* Device name */
	   int			request,	/* WE ID */
	   struct iwreq *	pwrq)		/* Fixed part of the request */
{
  /* Set device name */
  strncpy(pwrq->ifr_name, ifname, IFNAMSIZ);
  /* Do the request */
  return(ioctl(skfd, request, pwrq));
}



#endif /* CARDMODE_HH */
