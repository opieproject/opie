/* $Id: sniffer.hh,v 1.1 2002-11-12 22:12:33 mjm Exp $ */

#ifndef SNIFFER_HH
#define SNIFFER_HH

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>  
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/bpf.h>

#define   DEFAULT_PATH       "/proc/driver/aironet/%s/Config"
#define   CARD_TYPE_CISCO	1
#define   CARD_TYPE_NG	2
#define   CARD_TYPE_HOSTAP	3

#define   NONBROADCASTING "non-broadcasting"

/* only for now, until we have the daemon running */
/*the config file should provide these information */
#define   SNIFFER_DEVICE	 "wlan0"
#define CARD_TYPE CARD_TYPE_CISCO
#define   SBIN_PATH	"/sbin/ifconfig %s promisc up"
#define WLANCTL_PATH   "/sbin/wlanctl-ng"

/* holds all the interresting data */
struct packetinfo
{
	int isvalid;
	int pktlen;
	int fctype;
	int fcsubtype;
	int fc_wep;
	int cap_WEP;
	int cap_IBSS;
	int cap_ESS;
	int channel;
	char bssid[sizeof("00:00:00:00:00:00")];
	char desthwaddr[sizeof("00:00:00:00:00:00")];
	char sndhwaddr[sizeof("00:00:00:00:00:00")];
	char *ssid;	
	int ssid_len;
};


/* Prototypes */

int sniffer(void);
int card_into_monitormode (char * device, int cardtype);
int card_set_promisc_up (char * device);
int start_sniffing (char * device);
void process_packets(u_char *useless,const struct pcap_pkthdr* pkthdr,const u_char* packet);
int decode_80211b_hdr(const u_char *p,struct packetinfo *ppinfo);
void etheraddr_string(register const u_char *ep,char * text);
int handle_beacon(u_int16_t fc, const u_char *p,struct packetinfo *ppinfo);

static int GetHeaderLength(u_int16_t fc);

/*
 * True if  "l" bytes of "var" were captured.
 *
 * The "snapend - (l) <= snapend" checks to make sure "l" isn't so large
 * that "snapend - (l)" underflows.
 *
 * The check is for <= rather than < because "l" might be 0.
 */
#define TTEST2(var, l) (snapend - (l) <= snapend && \
			(const u_char *)&(var) <= snapend - (l))

/* True if "var" was captured */
#define TTEST(var) TTEST2(var, sizeof(var))

/* Bail if "l" bytes of "var" were not captured */
#define TCHECK2(var, l) if (!TTEST2(var, l)) goto trunc

/* Bail if "var" was not captured */
#define TCHECK(var) TCHECK2(var, sizeof(var))

#endif /* SNIFFER_HH */
