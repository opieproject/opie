/*
 * Communication protocol
 *
 * $Id: wl_proto.cc,v 1.1 2002-12-27 16:35:28 mjm Exp $
 */

#include "wl_proto.hh"
#include "wl_log.hh"
#include "wl_sock.hh"

/* Adds a field to the buffer */
int add_field(char *buffer, char *string, int len)
{
  char newlen[5];
  
  /* 3 Byte = Length */
  snprintf(newlen, sizeof(newlen) - 1, "%.3d", len);
  memcpy(buffer, newlen, 3);
  
  /* Length bytes = Value */
  memcpy(buffer + 3, string, atoi(newlen));
  
  /* Return length of attached field */
  return (atoi(newlen) + 3);
}

int get_field(const char *buffer, char *out)
{
  char len[5];

  /* Get length of value */
  memcpy(len, buffer, 3);

  /* Copy buffer to out pointer */
  memset(out, 0, atoi(len) + 1);
  memcpy(out, buffer + 3, atoi(len));

  return atoi(len) + 3;
}

/* Send found network to GUI */
int send_network_found (const char *guihost, int guiport, void *structure)
{
  wl_network_t *ptr;
  char buffer[2048];
  char temp[128];
  int retval=0, len=0;

  memset(temp, 0, sizeof(temp));

  ptr = (wl_network_t *)structure;

  /* Type = Found new net (without length field) */
  memset(temp, 0, sizeof(temp));
  snprintf(temp, sizeof(temp), "%.2d", NETFOUND);
  memcpy(buffer, temp, 2);
  len += 2;

  /* Set Net-type */
  memset(temp, 0, sizeof(temp));
  snprintf(temp, sizeof(temp), "%d", ptr->net_type);
  retval = add_field(buffer + len, temp, 1);
  len += retval;

  /* Set channel */
  memset(temp, 0, sizeof(temp));
  snprintf(temp, sizeof(temp), "%.2d", ptr->channel);
  retval = add_field(buffer + len, temp, 2); 
  len += retval;

  /* Set WEP y/n */
  memset(temp, 0, sizeof(temp));
  snprintf(temp, sizeof(temp), "%d", ptr->wep);
  retval = add_field(buffer + len, temp, 1);
  len += retval;

  /* Set Mac */
  retval = add_field(buffer + len, (char *)ptr->mac, 17);
  len += retval;

  /* Set ssid */
  retval = add_field(buffer + len, (char *)ptr->bssid, ptr->ssid_len);

  /* Send prepared buffer to GUI */
#ifdef DEBUG
  wl_loginfo("Sending network to GUI: '%s'", buffer);
#endif

  wl_send(guihost, guiport, buffer);

  return 1;
}

/* Fill buffer into structur */
int get_network_found (void *structure, const char *buffer)
{
  wl_network_t *ptr;
  char temp[512];
  int retval=0, len=0;

  ptr = (wl_network_t *)structure;
  
  /* packet type already determined */
  len += 2;

  /* Get net type */
  memset(temp, 0, sizeof(temp));
  retval = get_field(buffer + len, temp);
  len += retval;
  ptr->net_type = atoi(temp); 
   
  /* Get channel */
  memset(temp, 0, sizeof(temp));
  retval = get_field(buffer + len, temp);
  len += retval;
  ptr->channel = atoi(temp);

  /* Set WEP y/n */
  memset(temp, 0, sizeof(temp));
  retval = get_field(buffer + len, temp);
  len += retval;
  ptr->wep = atoi(temp);

  /* Set MAC address */
  retval = get_field(buffer + len, ptr->mac);
  len += retval;

  /* Set BSSID */
  retval = get_field(buffer + len, ptr->bssid);

  return 1;
}
