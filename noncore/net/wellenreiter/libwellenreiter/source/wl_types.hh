/* 
 * Global bufffer size and type definitions
 *
 * $Id: wl_types.hh,v 1.1 2002-12-31 12:50:44 mjm Exp $
 *
 */

#ifndef WL_TYPES_HH
#define WL_TYPES_HH

/* Type definitions, to be continued */
#define WL_NETFOUND 01
#define WL_NETLOST  02
#define WL_STARTSNIFF 98
#define WL_STOPSNIFF  99

#define WL_SOCKBUF 512 /* Buffer for wl_send and wl_recv calls */


/* WL network structure */
typedef struct {
  int net_type;    /* 1 = Accesspoint ; 2 = Ad-Hoc   */
  int ssid_len;    /* Length of SSID                 */
  int channel;     /* Channel                        */
  int wep;         /* 1 = WEP enabled ; 0 = disabled */
  char mac[64];    /* MAC address of Accesspoint     */
  char bssid[128]; /* BSSID of Net                   */
} wl_network_t;

#endif /* WL_TYPES_HH */

