/* 
 * Global bufffer size and type definitions
 *
 * $Id: wl_types.hh,v 1.2 2003-01-05 11:18:27 mjm Exp $
 *
 */

#ifndef WL_TYPES_HH
#define WL_TYPES_HH

/* Type definitions, to be continued */
#define WL_NETFOUND 01
#define WL_NETLOST  02
#define WL_STARTSNIFF 98
#define WL_STOPSNIFF  99

/* Socket specific */
#define WL_SOCKBUF 512 /* Buffer for wl_send and wl_recv calls */

/* Protocol specific */

/* WL network structure */
typedef struct {
  int net_type;    /* 1 = Accesspoint ; 2 = Ad-Hoc   */
  int ssid_len;    /* Length of SSID                 */
  int channel;     /* Channel                        */
  int wep;         /* 1 = WEP enabled ; 0 = disabled */
  char mac[64];    /* MAC address of Accesspoint     */
  char bssid[128]; /* BSSID of Net                   */
} wl_network_t;

/* Config specific */
#define WL_CONFFILE "sample.conf"
#define WL_CONFBUFF 128

#endif /* WL_TYPES_HH */

