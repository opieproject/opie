/* $Id: cardmode.hh,v 1.15 2003-02-09 20:50:34 max Exp $ */

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

/* only for now, until we have the daemon running */
/*the config file should provide these information */
#define   CARD_TYPE              CARD_TYPE_HOSTAP

/* Prototypes */
int card_check_rfmon_datalink (char *device);
int card_into_monitormode (pcap_t **, char *, int);
int card_set_promisc_up (const char *);
int card_set_channel (const char *device, int channel,int cardtype); 

#endif /* CARDMODE_HH */
