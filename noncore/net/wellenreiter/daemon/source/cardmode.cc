/* $Id: cardmode.cc,v 1.4 2002-11-23 21:42:41 mjm Exp $ */

#include "config.hh"
#include "cardmode.hh"

int card_into_monitormode (char *device, int cardtype)
{

	int datalink; /* used for getting the pcap datalink type */
	char CiscoRFMON[35] = "/proc/driver/aironet/";
	FILE *CISCO_CONFIG_FILE;
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *handle;
	
	/* Checks if we have a device to sniff on */
	if(device == NULL)
	{
		printf ("Fatal error i did not have any interfaces to sniff on\n");
		return 0;
	}

	/* Setting the prmiscous and up flag to the interface */
	if (card_set_promisc_up (device) == 0)
	{
		printf ("Interface flags correctly set using ifconfig\n");
	}

	/* Check the cardtype and executes the commands to go into monitor mode */
	if (cardtype == CARD_TYPE_CISCO) /* I got a cisco card */
	{
		/* bring the sniffer into rfmon  mode */
		snprintf(CiscoRFMON, sizeof(CiscoRFMON),DEFAULT_PATH, device);
		CISCO_CONFIG_FILE = fopen(CiscoRFMON,"w");
		fputs ("Mode: r",CISCO_CONFIG_FILE);
		fputs ("Mode: y",CISCO_CONFIG_FILE);
		fputs ("XmitPower: 1",CISCO_CONFIG_FILE);
		fclose(CISCO_CONFIG_FILE);
	}
	else if (cardtype == CARD_TYPE_NG)
	{
		char wlanngcmd[62];
		snprintf(wlanngcmd, sizeof(wlanngcmd),"%s %s lnxreq_wlansniff channel=1 enable=true",WLANCTL_PATH,device);
		if (system (wlanngcmd) != 0)
		{
			printf ("\n Fatal error could not set %s in raw mode, check cardtype\n\n\tterminating now...\n\n",device);
			exit(1);
	    }
	}
	else if (cardtype == CARD_TYPE_HOSTAP)
	{
		printf ("Got a host-ap card, nothing is implemented now\n");
	}

	
	/* Check the interface if it is in the correct raw mode */
    handle = pcap_open_live(device, BUFSIZ, 1, 0, errbuf);

	/* getting the datalink type */
    datalink = pcap_datalink(handle);

	if (datalink == DLT_IEEE802_11) /* Rawmode is IEEE802_11 */
	{
		printf ("Your successfully listen on %s in 802.11 raw mode\n",device);
		pcap_close(handle); 
		return 0;

	}
	else 
	{
		printf ("Fatal error, cannot continue, your interface %s does not work in the correct 802.11 raw mode, check you driver please\n\tterminating now",device);
		pcap_close(handle); 
		exit(1);
	}
}



int card_set_promisc_up (char * device)
{
	int ret;
	char ifconfigcmd[32];
	snprintf(ifconfigcmd,sizeof(ifconfigcmd),SBIN_PATH, device);
	ret = system (ifconfigcmd);
	if (ret > 0)
	{
		printf ("\nFatal error, could not execute %s please check your card,binary location and permission\n",ifconfigcmd);
		return 0;
	}
	return 1;
}
