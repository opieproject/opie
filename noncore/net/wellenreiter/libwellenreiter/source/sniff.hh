/* $Id: sniff.hh,v 1.3 2002-12-11 13:10:26 mickeyl Exp $ */

#ifndef SNIFF_HH
#define SNIFF_HH

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern "C"
{
#include <net/bpf.h>
#include <pcap.h>  
}

#define   NONBROADCASTING "non-broadcasting"

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
  char bssid[sizeof("00:00:00:00:00:00") + 1];
  char desthwaddr[sizeof("00:00:00:00:00:00") + 1];
  char sndhwaddr[sizeof("00:00:00:00:00:00") + 1];
  char ssid[128];
  int ssid_len;
};

void process_packets(const struct pcap_pkthdr* pkthdr,const u_char* packet, char *, int);
int decode_80211b_hdr(const u_char *p,struct packetinfo *ppinfo);
void etheraddr_string(register const u_char *ep,char * text);
int handle_beacon(u_int16_t fc, const u_char *p,struct packetinfo *ppinfo);

int GetHeaderLength(u_int16_t fc);

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

#endif /* SNIFF_HH */
