/*
 * Communication protocol
 *
 * $Id: proto.cc,v 1.3 2002-12-14 19:13:32 mjm Exp $
 */

#include "proto.hh"
#include "sock.hh"

/* Send found network to GUI */
int send_network_found (const char *guihost, int guiport, void *structure)
{
  wl_network_t *ptr;
  char buffer[2048];
  char temp[5];

  ptr = (wl_network_t *)structure;

  memset(buffer, 0, 2048);

  /* Type = Found new net */
  memcpy(buffer, "01", 2);

  /* Set Net-type */
  memset(temp, 0, sizeof(temp));
  snprintf(temp, 2, "%d", ptr->net_type);
  memcpy(buffer + 2, temp, 1);

  /* Set channel */
  memset(temp, 0, sizeof(temp));

  if(ptr->channel < 10)
    snprintf(temp, 3, "0%d", ptr->channel);
  else
    snprintf(temp, 3, "%d", ptr->channel);

  memcpy(buffer + 3, temp, 2);

  /* Set WEP y/n */
  memset(temp, 0, sizeof(temp));
  snprintf(temp, 2, "%d", ptr->wep);
  memcpy(buffer + 5, temp, 1);

  /* Set MAC address */
  memcpy(buffer + 6, ptr->mac, 17);

  /* Set lenght of ssid */
  memset(temp, 0, sizeof(temp));

  if(ptr->ssid_len > 99)
    snprintf(temp, 4, "%d", ptr->ssid_len);
  else if(ptr->ssid_len < 10)
    snprintf(temp, 4, "00%d", ptr->ssid_len);
  else
    snprintf(temp, 4, "0%d", ptr->ssid_len);

  memcpy(buffer + 23, temp, 3);

  /* Set ssid */
  memcpy(buffer + 26, ptr->bssid, ptr->ssid_len);

  /* Send prepared buffer to GUI */
#ifdef DEBUG
  wl_loginfo("Sending network to GUI: '%s'", buffer);
#endif

  sendcomm(guihost, guiport, buffer);

  return 1;
}

/* Fill buffer into structur */
int get_network_found (void *structure, const char *buffer)
{
  wl_network_t *ptr;
  char temp[512];

  ptr = (wl_network_t *)structure;

  /* Get net type */
  memset(temp, 0, sizeof(temp));
  memcpy(temp, buffer + 2, 1);
  ptr->net_type = atoi(temp);

  /* Get channel */
  memset(temp, 0, sizeof(temp));
  memcpy(temp, buffer + 3, 2);
  ptr->channel = atoi(temp);

  /* Set WEP y/n */
  memset(temp, 0, sizeof(temp));
  memcpy(temp, buffer + 5, 1);
  ptr->wep = atoi(temp);

  /* Set MAC address */
  memcpy(ptr->mac, buffer + 6, 17);
  ptr->mac[17]='\0';

  /* Set lenght of ssid */
  memset(temp, 0, sizeof(temp));
  memcpy(temp, buffer + 23, 3);
  ptr->ssid_len = atoi(temp);

  /* Set ssid */
  memcpy(ptr->bssid, buffer + 26, ptr->ssid_len + 1);

  return 1;
}
