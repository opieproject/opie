/* $Id: wl_proto.hh,v 1.1 2002-12-27 16:35:28 mjm Exp $ */

#ifndef WLPROTO_HH
#define WLPROTO_HH

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Type definitions, to be continued */
#define NETFOUND 01
#define NETLOST  02
#define STARTSNIFF 98
#define STOPSNIFF  99

int send_network_found (const char *, int, void *);
int get_network_found (void *, const char *);

typedef struct {
  int net_type;    /* 1 = Accesspoint ; 2 = Ad-Hoc   */
  int ssid_len;    /* Length of SSID                 */
  int channel;     /* Channel                        */
  int wep;         /* 1 = WEP enabled ; 0 = disabled */
  char mac[64];    /* MAC address of Accesspoint     */
  char bssid[128]; /* BSSID of Accesspoint           */
} wl_network_t;

#endif /* WLPROTO_HH */
