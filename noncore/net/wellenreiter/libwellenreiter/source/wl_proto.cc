/*
 * Communication protocol
 *
 * $Id: wl_proto.cc,v 1.7 2003-01-05 11:18:27 mjm Exp $
 */

#include "wl_types.hh"
#include "wl_proto.hh"
#include "wl_log.hh"
#include "wl_sock.hh"

/* Adds a field to the buffer */
int add_field(char *buffer, const char *string, int len)
{
  char newlen[5];
  
  /* 3 Byte = Length */
  memset(newlen, 0, sizeof(newlen));
  snprintf(newlen, sizeof(newlen) - 1, "%.3d", len);
  memcpy(buffer, newlen, 3);
  
  /* Length bytes = Value */
  memcpy(buffer + 3, string, atoi(newlen));
  
  /* Return length of attached field */
  return (atoi(newlen) + 3);
}

int get_field(const char *buffer, char *out, int maxlen)
{
  char len[5];

  /* Get length of value */
  memset(len, 0, sizeof(len));
  memcpy(len, buffer, 3);

  /* Copy buffer to out pointer */
  memset(out, 0, maxlen);
  if(atoi(len) > maxlen -1)
    memcpy(out, buffer + 3, maxlen - 1);
  else
    memcpy(out, buffer + 3, atoi(len));

  /* Return length of whole field (including 3 byte length) */
  return (atoi(len) + 3);
}

/* Send found network to UI */
int send_network_found (const char *guihost, int guiport, void *structure)
{
  wl_network_t *ptr;
  char buffer[2048], temp[5];
  unsigned int len = 0;

  ptr = (wl_network_t *)structure;

  memcpy(buffer, 0, sizeof(buffer));

  /* Type = Found new net (without length field) */
  memset(temp, 0, sizeof(temp));
  snprintf(temp, sizeof(temp), "%.2d", WL_NETFOUND);
  memcpy(buffer, temp, 2);
  len += 2;

  /* Set Net-type */
  memset(temp, 0, sizeof(temp));
  snprintf(temp, sizeof(temp), "%d", ptr->net_type);
  len += add_field(buffer + len, temp, 1);

  /* Set channel */
  memset(temp, 0, sizeof(temp));
  snprintf(temp, sizeof(temp), "%.2d", ptr->channel);
  len += add_field(buffer + len, temp, 2); 

  /* Set WEP y/n */
  memset(temp, 0, sizeof(temp));
  snprintf(temp, sizeof(temp), "%d", ptr->wep);
  len += add_field(buffer + len, temp, 1);

  /* Set Mac */
  len += add_field(buffer + len, ptr->mac, 17);

  /* Set ssid */
  if(len + ptr->ssid_len < sizeof(buffer) - 1)
    len += add_field(buffer + len, ptr->bssid, ptr->ssid_len);
  else
    len += add_field(buffer + len, ptr->bssid, sizeof(buffer) - len - 1);

  /* Send prepared buffer to UI */
#ifdef DEBUG
  wl_loginfo("Sending network to UI: '%s'", buffer);
#endif

  return ((!wl_send(guihost, guiport, buffer)) ? 0 : 1);
}

/* Fill buffer into structur */
int get_network_found (void *structure, const char *buffer)
{
  wl_network_t *ptr;
  char temp[5];
  unsigned int len = 0;

  ptr = (wl_network_t *)structure;

  /* packet type already determined, skip check */
  len += 2;

  /* Get net type (accesspoint || ad-hoc || ...) */
  memset(temp, 0, sizeof(temp));
  len += get_field(buffer + len, temp, sizeof(temp));
  ptr->net_type = atoi(temp); 
   
  /* Get channel */
  memset(temp, 0, sizeof(temp));
  len += get_field(buffer + len, temp, sizeof(temp));
  ptr->channel = atoi(temp);

  /* Set WEP y/n */
  memset(temp, 0, sizeof(temp));
  len += get_field(buffer + len, temp, sizeof(temp));
  ptr->wep = atoi(temp);

  /* Set MAC address */
  len += get_field(buffer + len, ptr->mac, sizeof(ptr->mac));

  /* Set BSSID */
  len += get_field(buffer + len, ptr->bssid, sizeof(ptr->bssid));

  return 1;
}
