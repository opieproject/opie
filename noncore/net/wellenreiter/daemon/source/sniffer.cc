/* 
 *  rfmon mode sniffer
 *  This works only with cisco wireless cards with an rfmon
 *  able driver and not with wifi stuff.
 *
 *  $Id: sniffer.cc,v 1.2 2002-11-23 20:12:57 max Exp $
 */

#include "config.hh"
#include "cardmode.hh"
#include "sniffer.hh"
#include "ieee802_11.hh"
#include "extract.hh"

int main(void)
{	
	if(card_into_monitormode (SNIFFER_DEVICE, CARD_TYPE_NG) < 0)
		return 0;
	start_sniffing (SNIFFER_DEVICE);	

  return 1;
}

int start_sniffing (char * device)
{
 
	pcap_t *handletopcap;
	char errbuf[PCAP_ERRBUF_SIZE];

	/* opening the pcap for sniffing */
	handletopcap = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);

	#ifdef HAVE_PCAP_NONBLOCK
    	pcap_setnonblock(handletopcap, 1, errstr);
	#endif
	/*start scanning */
	pcap_loop(handletopcap,-1,process_packets,NULL);

	printf("\nDone processing packets... wheew!\n");
	return 1;
}

void process_packets(u_char *useless,const struct pcap_pkthdr* pkthdr,const u_char* packet)
{
	u_int caplen = pkthdr->caplen;
	u_int length = pkthdr->len;
	u_int16_t fc;
	u_int HEADER_LENGTH;

	/* pinfo holds all interresting information for us */
	struct packetinfo pinfo;
	struct packetinfo *pinfoptr;
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
	else
	{   /* Something is wrong,could not be a correct packet */
		return;
	}
	
	switch (FC_TYPE(fc)) 
	{
		/* Is it a managemnet frame? */
		case T_MGMT:
			switch (FC_SUBTYPE(fc))
			{   /* Is it a beacon frame? */
				case ST_BEACON:
					if (handle_beacon(fc, packet,pinfoptr) ==0)
					{
						printf ("\n\tOn network : %s",pinfoptr->ssid);
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
							printf ("\nHave found an accesspoint:");
						}
						else if(pinfoptr->cap_ESS == 0 && pinfoptr->cap_IBSS == 1) 
						{
							printf ("\nHave found an AD-HOC station:");
							
						}
						if (strcmp (pinfoptr->ssid,NONBROADCASTING) ==0)
						{
							printf ("\n\tOn a non-broadcasting network");
						}
						else
						{
							printf ("\n\tOn network : %s",pinfoptr->ssid);
						}
							printf ("\n\tLen SSID   : %d",pinfoptr->ssid_len);
							printf ("\n\tOn Channel : %d",pinfoptr->channel);
							printf ("\n\tEncryption : %s", pinfoptr->cap_WEP ? "ON" : "OFF");
							printf ("\n\tMacaddress : %s",pinfoptr->sndhwaddr);
							printf ("\n\tBssid      : %s",pinfoptr->bssid);
							printf ("\n\tDest	   : %s\n",pinfoptr->desthwaddr);
					}
				break;	
				default:
					printf("Unknown IEEE802.11 frame subtype (%d)",FC_SUBTYPE(fc));
				break;	
			} /* End of switch over different mgt frame types */
						
			break;
		case T_CTRL:
			//	decode_control_frames(fc, packet);
			printf ("Its a control frame");
			break;
		case T_DATA:
		//	decode_data_frames(fc, packet);
			printf ("Its a date frame");
			break;
		default:
			printf("Unknown IEEE802.11 frame type (%d)",FC_TYPE(fc));
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


void etheraddr_string(register const u_char *ep,char * text)
{
	static char hex[] = "0123456789abcdef";
	register u_int i, j;
	register char *cp;
	char buf[sizeof("00:00:00:00:00:00")];
	cp = buf;
	if ((j = *ep >> 4) != 0)
		*cp++ = hex[j];
	*cp++ = hex[*ep++ & 0xf];
	for (i = 5; (int)--i >= 0;) {
		*cp++ = ':';
		if ((j = *ep >> 4) != 0)
			*cp++ = hex[j];
		*cp++ = hex[*ep++ & 0xf];
	}
	*cp = '\0';
	strcpy(text,buf);
}

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
					{
						ppinfo->ssid = NONBROADCASTING;
					}
					else
					{
						ppinfo->ssid = (char *)pbody.ssid.ssid;
					}
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
				if (pbody.rates.length > 0) {
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


static int GetHeaderLength(u_int16_t fc)
{
	int iLength=0;

	switch (FC_TYPE(fc)) {
	case T_MGMT:
		iLength = MGMT_HEADER_LEN;
		break;
	case T_CTRL:
		switch (FC_SUBTYPE(fc)) {
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
		printf("unknown IEEE802.11 frame type (%d)",
		    FC_TYPE(fc));
		break;
	}

	return iLength;
}
