/* $Id: proto.hh,v 1.1 2002-11-27 21:17:27 mjm Exp $ */

#ifndef PROTO_HH
#define PROTO_HH

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

#endif /* PROTO_HH */
