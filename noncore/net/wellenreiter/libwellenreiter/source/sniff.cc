/* 
 *  rfmon mode sniffer
 *
 *  $Id: sniff.cc,v 1.10 2002-12-28 16:57:51 mjm Exp $
 */

#include "sniff.hh"
#include "ieee802_11.hh"
#include "extract.hh"
#include "wl_log.hh"
#include "wl_proto.hh"

/* Main function, checks packets */
void process_packets(const struct pcap_pkthdr *pkthdr, const unsigned char *packet, char *guihost, int guiport)
{
  unsigned int caplen = pkthdr->caplen;
  unsigned int length = pkthdr->len;
  u_int16_t fc;
  unsigned int HEADER_LENGTH;
  
  /* pinfo holds all interresting information for us */
  struct packetinfo pinfo;
  struct packetinfo *pinfoptr;

  /* wl_network_t will finally be set and send to the ui */
  wl_network_t wl_net;

  pinfoptr=&pinfo;
  
  pinfoptr->isvalid = 0;
  pinfoptr->pktlen = pkthdr->len;
  
  if (caplen < IEEE802_11_FC_LEN) 
  {
      /* This is a garbage packet, because is does not long enough
	 to hold a 802.11b header */
      pinfoptr->isvalid = 0;
      return;
  }
  
  /* Gets the framecontrol bits (2bytes long) */
  fc = EXTRACT_LE_16BITS(packet);
  
  HEADER_LENGTH = GetHeaderLength(fc);
  
  if (caplen < HEADER_LENGTH) 
  {
      /* This is a garbage packet, because it is not long enough
	 to hold a correct header of its type */
      pinfoptr->isvalid = 0;
      return;
  }
  
  /* Decode 802.11b header out of the packet */
  if (decode_80211b_hdr(packet,pinfoptr) == 0)
  {
      /* Justification of the ofset to further process the packet */
      length -= HEADER_LENGTH;
      caplen -= HEADER_LENGTH;
      packet += HEADER_LENGTH;
  }
  else /* Something is wrong,could not be a correct packet */
    return;
  
  switch (FC_TYPE(fc)) 
  {
      /* Is it a managemnet frame? */
    case T_MGMT:
      switch (FC_SUBTYPE(fc))
      {
	case ST_BEACON:
	  if (handle_beacon(fc, packet,pinfoptr) ==0)
	  {
	      if (!strcmp(pinfoptr->desthwaddr,"ff:ff:ff:ff:ff:ff") == 0)
	      {
		  /* Every beacon must have the broadcast as destination
		     so it must be a shitti packet */
		  pinfoptr->isvalid = 0;
		  return;
	      } 
	      
	      if (pinfoptr->cap_ESS == pinfoptr->cap_IBSS)
	      {
		  /* Only one of both are possible, so must be 
		     a noise packet, if this comes up */
		  pinfoptr->isvalid = 0;
		  return;
	      }
	      if (pinfoptr->channel < 1 || pinfoptr->channel > 14)
	      {
		  /* Only channels between 1 and 14 are possible
		     others must be noise packets */
		  pinfoptr->isvalid = 0;
		  return;
	      }

	      /* Here should be the infos to the gui issued */
	      if (pinfoptr->cap_ESS == 1 &&pinfoptr->cap_IBSS ==0)
		{
		  wl_loginfo("Found an access point");
		  wl_net.net_type=1;
		}
	      else if(pinfoptr->cap_ESS == 0 && pinfoptr->cap_IBSS == 2) 
		{
		  wl_loginfo("Found an ad-hoc network");
		  wl_net.net_type=2;
		}

	      memset(wl_net.bssid, 0, sizeof(wl_net.bssid));

	      if (strcmp (pinfoptr->ssid,NONBROADCASTING) ==0)
		  wl_loginfo("Net is a non-broadcasting network");
	      else
		  wl_loginfo("SSID is: %s", pinfoptr->ssid);

              wl_loginfo("SSID is: %s", pinfoptr->ssid);
	      memcpy(wl_net.bssid, pinfoptr->ssid, sizeof(wl_net.bssid)-1);

	      wl_loginfo("SSID length is: %d", pinfoptr->ssid_len);
	      wl_net.ssid_len=pinfoptr->ssid_len;
	    
              wl_loginfo("Channel is: %d", pinfoptr->channel);
	      wl_net.channel=pinfoptr->channel;
	      wl_net.wep=pinfoptr->cap_WEP;

	      wl_loginfo("Mac is: %s", pinfoptr->sndhwaddr);
	      memset(wl_net.mac, 0, sizeof(wl_net.mac));
	      memcpy(wl_net.mac, pinfoptr->sndhwaddr, sizeof(wl_net.mac)-1);
	      
	      if(!send_network_found((char *)guihost, guiport, &wl_net))
	      {
		wl_logerr("Error sending data to UI: %s", strerror(errno));
		break;
	      }
	      wl_loginfo("Sent network to GUI '%s:%d'", guihost, guiport);
	    }
	  break;	

	default:
	  wl_logerr("Unknown IEEE802.11 frame subtype (%d)", FC_SUBTYPE(fc));
	  break;	
	} /* End of switch over different mgt frame types */
      
      break;
  
  case T_CTRL:
    wl_loginfo("Received control frame, not implemented yet");
    break;

  case T_DATA:
    wl_loginfo("Received date frame, not implemented yet");
    break;

  default:
    wl_logerr("Unknown IEEE802.11 frame type (%d)", FC_TYPE(fc));
    break;
    }
}

/* This decodes the 802.11b frame header out of the 802.11b packet 
   all the infos is placed into the packetinfo structure			 */
int decode_80211b_hdr(const u_char *p,struct packetinfo *ppinfo)
{
  const struct mgmt_header_t *mgthdr = (const struct mgmt_header_t *) p;
  ppinfo->fcsubtype = FC_SUBTYPE(mgthdr->fc);
  
  /* Get the sender, bssid and dest mac address */
  etheraddr_string(mgthdr->bssid,ppinfo->bssid);
  etheraddr_string(mgthdr->da,ppinfo->desthwaddr);
  etheraddr_string(mgthdr->sa,ppinfo->sndhwaddr);
  ppinfo->fc_wep = FC_WEP(mgthdr->fc);
  return 0;
}


void etheraddr_string(register const u_char *ep, char *text)
{
  static char hex[] = "0123456789abcdef";
  register unsigned int i, j;
  register char *cp;
  char buf[sizeof("00:00:00:00:00:00\0")];
  cp = buf;
  if ((j = *ep >> 4) != 0)
  {
  	*cp++ = hex[j];
  }
  else
  {
  	*cp++ = '0';
  }
  *cp++ = hex[*ep++ & 0xf];
  
  for (i = 5; (int)--i >= 0;) 
  {
    *cp++ = ':';
    if ((j = *ep >> 4) != 0)
    {
    	*cp++ = hex[j];
    }
    else
    {
    	*cp++ = '0';
    }
  
    *cp++ = hex[*ep++ & 0xf];
  }
  *cp = '\0';
  strcpy(text,buf);
}

/* beacon handler */
int handle_beacon(u_int16_t fc, const u_char *p,struct packetinfo *ppinfo)
{
  struct mgmt_body_t pbody;
  int offset = 0;
  
  /* Get the static informations out of the packet */
  memset(&pbody, 0, sizeof(pbody));
  memcpy(&pbody.timestamp, p, 8);
  offset += 8;
  pbody.beacon_interval = EXTRACT_LE_16BITS(p+offset);
  offset += 2;
  pbody.capability_info = EXTRACT_LE_16BITS(p+offset);
  offset += 2;
  
  /* Gets the different flags out of the capabilities */
  ppinfo->cap_ESS = CAPABILITY_ESS(pbody.capability_info);
  ppinfo->cap_IBSS = CAPABILITY_IBSS(pbody.capability_info);
  ppinfo->cap_WEP = CAPABILITY_PRIVACY(pbody.capability_info);
  
  /* Gets the tagged elements out of the packets */
  while (offset + 1 < ppinfo->pktlen)
  {
      switch (*(p + offset)) 
      {
	case E_SSID:
	  memcpy(&(pbody.ssid),p+offset,2); offset += 2;
	  if (pbody.ssid.length > 0)
	  {
	      memcpy(&(pbody.ssid.ssid),p+offset,pbody.ssid.length); offset += pbody.ssid.length;
	      pbody.ssid.ssid[pbody.ssid.length]='\0';
	      if (strcmp((char *)pbody.ssid.ssid,"")==0)
		memcpy(ppinfo->ssid, NONBROADCASTING, sizeof(ppinfo->ssid));
	      else
		  memcpy(ppinfo->ssid, pbody.ssid.ssid, sizeof(ppinfo->ssid));
	      ppinfo->ssid_len = pbody.ssid.length;
	  }
	  break;
	  
	case E_CHALLENGE:
	  memcpy(&(pbody.challenge),p+offset,2); offset += 2;
	  if (pbody.challenge.length > 0)
	  {
	      memcpy(&(pbody.challenge.text),p+offset,pbody.challenge.length); offset += pbody.challenge.length;
	      pbody.challenge.text[pbody.challenge.length]='\0';
	  }
	  break;
	case E_RATES:
	  memcpy(&(pbody.rates),p+offset,2); offset += 2;
	  if (pbody.rates.length > 0) 
	  {
	    memcpy(&(pbody.rates.rate),p+offset,pbody.rates.length); offset += pbody.rates.length;
	  }
	  break;
	case E_DS:
	  memcpy(&(pbody.ds),p+offset,3); offset +=3;
	  ppinfo->channel = pbody.ds.channel;
	  break;
	case E_CF:
	  memcpy(&(pbody.cf),p+offset,8); offset +=8;
	  break;
	case E_TIM:
	  memcpy(&(pbody.tim),p+offset,2); offset +=2;
	  memcpy(&(pbody.tim.count),p+offset,3); offset +=3;
	  if ((pbody.tim.length -3) > 0)
	  {
	      memcpy((pbody.tim.bitmap),p+(pbody.tim.length -3),(pbody.tim.length -3));
	      offset += pbody.tim.length -3;
	  }
	  break;
	default:
	  
	  offset+= *(p+offset+1) + 2;
	  break;
	} /* end of switch*/
    } /* end of for loop */
  return 0;
  
} /* End of handle_beacon */


int GetHeaderLength(u_int16_t fc)
{
  int iLength=0;
  
  switch (FC_TYPE(fc)) 
  {
    case T_MGMT:
      iLength = MGMT_HEADER_LEN;
      break;
    case T_CTRL:
      switch (FC_SUBTYPE(fc)) 
      {
        case CTRL_PS_POLL:
	  iLength = CTRL_PS_POLL_LEN;
	  break;
        case CTRL_RTS:
	  iLength = CTRL_RTS_LEN;
	  break;
        case CTRL_CTS:
	  iLength = CTRL_CTS_LEN;
	  break;
        case CTRL_ACK:
	  iLength = CTRL_ACK_LEN;
	  break;
        case CTRL_CF_END:
	  iLength = CTRL_END_LEN;
	  break;
        case CTRL_END_ACK:
	  iLength = CTRL_END_ACK_LEN;
	  break;
        default:
	  iLength = 0;
	  break;
      }
      break;
    case T_DATA:
      if (FC_TO_DS(fc) && FC_FROM_DS(fc))
	iLength = 30;
      else
	iLength = 24;
      break;
    default:
      wl_logerr("unknown IEEE802.11 frame type (%d)", FC_TYPE(fc));
      break;
  }
  return iLength;
}
