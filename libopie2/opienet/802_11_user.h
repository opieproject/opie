#ifndef IEEE_802_11
#define IEEE_802_11

enum ieee_802_11_link_status_failure_reason {
	reserved0, Unspecified=1, Previous_not_valid,
	Sender_Quits_ESS_or_IBSS,
	Due_Inactivity, AP_Overload,
	Class_2_from_NonAuth,
	Class_3_from_NonAuth,
	Sender_Quits_BSS,
	Association_requester_not_authenticated,
	Reserved10
};


#define IEEE_802_11_LINK_STATUS_FAILURE_REASON_STRINGS \
{	\
        {reserved0,		0xff," Reserved reason "},\
        {Unspecified,		0xff," Unspecified Reason "},\
        {Previous_not_valid,	0xff," Previous Authentication no longer valid "},\
        {Sender_Quits_ESS_or_IBSS,0xff," Deauthenticated because sending station is leaving (has left) IBSS or ESS "},\
        {Due_Inactivity,	0xff," Disassociated due to inactivity "},\
        {AP_Overload,		0xff," Disassociated because AP is unable to handle all currently associated stations "},\
        {Class_2_from_NonAuth,	0xff," Class 2 frame received from non-Authenticated station"},\
        {Class_3_from_NonAuth,	0xff," Class 3 frame received from non­Associated station"},\
        {Sender_Quits_BSS,	0xff," Disassociated because sending station is leaving (has left) BSS"},\
        {Association_requester_not_authenticated,0xff," Station requesting (Re)Association is not Authenticated with responding station"},\
        {Reserved10,		0xff," Reserved"},\
	{0,0,NULL}\
};



struct ieee_802_11_header {
	u_int16_t	frame_control;// needs to be subtyped
	u_int16_t	duration;
	u_int8_t	mac1[6];
	u_int8_t	mac2[6];
	u_int8_t	mac3[6];
	u_int16_t	SeqCtl;
	u_int8_t	mac4[6];
//	u_int16_t	gapLen;
//	u_int8_t	gap[8];
};


struct ieee_802_3_header {

	u_int16_t	status;
	u_int16_t	payload_length;
	u_int8_t	dst_mac[6];
	u_int8_t	src_mac[6];

};

#define P80211_OUI_LEN 3

struct ieee_802_11_snap_header {

	u_int8_t    dsap;   /* always 0xAA */
	u_int8_t    ssap;   /* always 0xAA */
	u_int8_t    ctrl;   /* always 0x03 */
	u_int8_t    oui[P80211_OUI_LEN];    /* organizational universal id */

} __attribute__ ((packed));

#define P80211_LLC_OUI_LEN 3

struct ieee_802_11_802_1H_header {

	u_int8_t    dsap;
	u_int8_t    ssap;   /* always 0xAA */
	u_int8_t    ctrl;   /* always 0x03 */
	u_int8_t    oui[P80211_OUI_LEN];    /* organizational universal id */
	u_int16_t    unknown1;      /* packet type ID fields */
	u_int16_t    unknown2;		/* here is something like length in some cases */
} __attribute__ ((packed));

struct ieee_802_11_802_2_header {

	u_int8_t    dsap;
	u_int8_t    ssap;   /* always 0xAA */
	u_int8_t    ctrl;   /* always 0x03 */
	u_int8_t    oui[P80211_OUI_LEN];    /* organizational universal id */
	u_int16_t   type;      /* packet type ID field */

} __attribute__ ((packed));



// following is incoplete and may be incorrect and need reorganization

#define ieee_802_11_frame_type_Management	0x00
#define ieee_802_11_frame_type_Control		0x01
#define ieee_802_11_frame_type_Data		0x10
#define ieee_802_11_frame_type_Reserved		0x11

#define ieee_802_11_frame_subtype_Association_Req	0x0 // Association Request
#define ieee_802_11_frame_subtype_Association_Resp	0x1 // Association Response
#define ieee_802_11_frame_subtype_Reassociation_Req	0x2 // Reassociation Request
#define ieee_802_11_frame_subtype_Reassociation_Resp	0x3 // Reassociation Response
#define ieee_802_11_frame_subtype_Probe_Req		0x4 // Probe Request
#define ieee_802_11_frame_subtype_Probe_Resp		0x5 // Probe Response
#define ieee_802_11_frame_subtype_Beacon 		0x8 // Beacon
#define ieee_802_11_frame_subtype_ATIM 			0x9 // ATIM
#define ieee_802_11_frame_subtype_Disassociation 	0xA // Disassociation
#define ieee_802_11_frame_subtype_Authentication 	0xB // Authentication
#define ieee_802_11_frame_subtype_Deauthentication 	0xC // Deauthentication
#define ieee_802_11_frame_subtype_PS_Poll 		0xA // PS-Poll
#define ieee_802_11_frame_subtype_RTS 			0xB // RTS
#define ieee_802_11_frame_subtype_CTS 			0xC // CTS
#define ieee_802_11_frame_subtype_ACK 			0xD // ACK
#define ieee_802_11_frame_subtype_CFEnd 		0xE // CF-End
#define ieee_802_11_frame_subtype_CFEnd_CFAck 		0xF // CF-End + CF-Ack
#define ieee_802_11_frame_subtype_Data 			0x0 // Data
#define ieee_802_11_frame_subtype_Data_CFAck 		0x1 // Data + CF-Ack
#define ieee_802_11_frame_subtype_Data_CF_Poll 		0x2 // Data + CF-Poll
#define ieee_802_11_frame_subtype_Data_CF_AckCF_Poll 	0x3 // Data + CF-Ack + CF-Poll
#define ieee_802_11_frame_subtype_NullFunction 		0x4 // Null Function (no data)
#define ieee_802_11_frame_subtype_CF_Ack 		0x5 // CF-Ack (no data)
#define ieee_802_11_frame_subtype_CF_Poll 		0x6 // CF-Poll (no data)
#define ieee_802_11_frame_subtype_CF_AckCF_Poll 	0x7 // CF-Ack + CF-Poll (no data)


#define ieee_802_11_frame_subtype_strings {\
	{ ieee_802_11_frame_subtype_Association_Req,	0xF,"f  Association Request"},\
	{ ieee_802_11_frame_subtype_Association_Resp,	0xF,"1  Association Response"},\
	{ ieee_802_11_frame_subtype_Reassociation_Req,	0xF,"2  Reassociation Request"},\
	{ ieee_802_11_frame_subtype_Reassociation_Resp,	0xF,"3  Reassociation Response"},\
	{ ieee_802_11_frame_subtype_Probe_Req	,	0xF,"4  Probe Request"},\
	{ ieee_802_11_frame_subtype_Probe_Resp	,	0xF,"5  Probe Response"},\
	{ ieee_802_11_frame_subtype_Beacon 	,	0xF,"8  Beacon"},\
	{ ieee_802_11_frame_subtype_ATIM 	,	0xF,"9  ATIM"},\
	{ ieee_802_11_frame_subtype_Disassociation,	0xF,"A  Disassociation"},\
	{ ieee_802_11_frame_subtype_Authentication,	0xF,"B  Authentication"},\
	{ ieee_802_11_frame_subtype_Deauthentication,	0xF,"C  Deauthentication"},\
	{ ieee_802_11_frame_subtype_PS_Poll 	,	0xF,"A  PS-Poll"},\
	{ ieee_802_11_frame_subtype_RTS 	,	0xF,"B  RTS"},\
	{ ieee_802_11_frame_subtype_CTS 	,	0xF,"C  CTS"},\
	{ ieee_802_11_frame_subtype_ACK 	,	0xF,"D  ACK"},\
	{ ieee_802_11_frame_subtype_CFEnd	,	0xF,"E  CF-End"},\
	{ ieee_802_11_frame_subtype_CFEnd_CFAck ,	0xF,"F  CF-End + CF-Ack"},\
	{ ieee_802_11_frame_subtype_Data 	,	0xF,"0  Data"},\
	{ ieee_802_11_frame_subtype_Data_CFAck 	,	0xF,"1  Data + CF-Ack"},\
	{ ieee_802_11_frame_subtype_Data_CFPoll ,	0xF,"2  Data + CF-Poll"},\
	{ ieee_802_11_frame_subtype_Data_CFAck_CFPoll,	0xF,"3  Data + CF-Ack + CF-Poll"},\
	{ ieee_802_11_frame_subtype_Null_Function ,	0xF,"4  Null Function (no data)"},\
	{ ieee_802_11_frame_subtype_CFAck ,		0xF,"5  CF-Ack (no data)"},\
	{ ieee_802_11_frame_subtype_CFPoll ,		0xF,"6  CF-Poll (no data)"},\
	{ ieee_802_11_frame_subtype_CFAck_CFPoll,	0xF,"y7  CF-Ack + CF-Poll (no data)"},\
	{ 0,0,NULL}\
}
struct ieee_802_11_frame_subtype_class {
	u_int8_t	subtype;
	u_int8_t	mask;
	u_int8_t	klass;
	u_int8_t	type;
};
#define ieee_802_11_frame_subtype_classes {\
	{ ieee_802_11_frame_subtype_Association_Req,	0xF,2,ieee_802_11_frame_type_Management},\
	{ ieee_802_11_frame_subtype_Association_Resp,	0xF,2,ieee_802_11_frame_type_Management},\
	{ ieee_802_11_frame_subtype_Reassociation_Req,	0xF,2,ieee_802_11_frame_type_Management},\
	{ ieee_802_11_frame_subtype_Reassociation_Resp,	0xF,2,ieee_802_11_frame_type_Management},\
	{ ieee_802_11_frame_subtype_Probe_Req	,	0xF,1,ieee_802_11_frame_type_Management},\
	{ ieee_802_11_frame_subtype_Probe_Resp	,	0xF,1,ieee_802_11_frame_type_Management},\
	{ ieee_802_11_frame_subtype_Beacon 	,	0xF,1,ieee_802_11_frame_type_Management},\
	{ ieee_802_11_frame_subtype_ATIM 	,	0xF,1,ieee_802_11_frame_type_Management},\
	{ ieee_802_11_frame_subtype_Disassociation,	0xF,2,ieee_802_11_frame_type_Management},\
	{ ieee_802_11_frame_subtype_Authentication,	0xF,1,ieee_802_11_frame_type_Management},\
	{ ieee_802_11_frame_subtype_Deauthentication,	0xF,3,ieee_802_11_frame_type_Management},\
	{ ieee_802_11_frame_subtype_PS-Poll 	,	0xF,3,ieee_802_11_frame_type_Control},\
	{ ieee_802_11_frame_subtype_RTS 	,	0xF,1,ieee_802_11_frame_type_Control},\
	{ ieee_802_11_frame_subtype_CTS 	,	0xF,1,ieee_802_11_frame_type_Control},\
	{ ieee_802_11_frame_subtype_ACK 	,	0xF,1,ieee_802_11_frame_type_Control},\
	{ ieee_802_11_frame_subtype_CFEnd	,	0xF,1,ieee_802_11_frame_type_Control},\
	{ ieee_802_11_frame_subtype_CFEnd_CFAck ,	0xF,1,ieee_802_11_frame_type_Control},\
	{ ieee_802_11_frame_subtype_Data 	,	0xF,3,ieee_802_11_frame_type_Data},\
	{ ieee_802_11_frame_subtype_Data_CFAck 	,	0xF,3,ieee_802_11_frame_type_Data},\
	{ ieee_802_11_frame_subtype_Data_CF_Poll 	0xF,3,ieee_802_11_frame_type_Data},\
	{ ieee_802_11_frame_subtype_Data_CF_AckCF_Poll,	0xF,3,ieee_802_11_frame_type_Data},\
	{ ieee_802_11_frame_subtype_NullFunction 	0xF,1,ieee_802_11_frame_type_Data},\
	{ ieee_802_11_frame_subtype_CF_Ack ,		0xF,1,ieee_802_11_frame_type_Data},\
	{ ieee_802_11_frame_subtype_CF_Poll ,		0xF,1,ieee_802_11_frame_type_Data},\
	{ ieee_802_11_frame_subtype_CF_AckCF_Poll,	0xF,1,ieee_802_11_frame_type_Data},\
	{ 0,0,NULL}\
}

#define IEEE802_11_FC_LEN	2

#define T_MGMT 0x0  /* management */
#define T_CTRL 0x1  /* control */
#define T_DATA 0x2 /* data */
#define T_RESV 0x3  /* reserved */

#define ST_ASSOC_REQUEST   	0x0
#define ST_ASSOC_RESPONSE 	0x1
#define ST_REASSOC_REQUEST   	0x2
#define ST_REASSOC_RESPONSE  	0x3
#define ST_PROBE_REQUEST   	0x4
#define ST_PROBE_RESPONSE   	0x5
/* RESERVED 			0x6  */
/* RESERVED 			0x7  */
#define ST_BEACON   		0x8
#define ST_ATIM			0x9
#define ST_DISASSOC		0xA
#define ST_AUTH			0xB
#define ST_DEAUTH		0xC
/* RESERVED 			0xD  */
/* RESERVED 			0xE  */
/* RESERVED 			0xF  */


#define CTRL_PS_POLL	0xA
#define CTRL_RTS	0xB
#define CTRL_CTS	0xC
#define CTRL_ACK	0xD
#define CTRL_CF_END	0xE
#define CTRL_END_ACK	0xF

/*
 * Bits in the frame control field.
 */
#define FC_VERSION(fc)		((fc) & 0x3)
#define FC_TYPE(fc)		(((fc) >> 2) & 0x3)
#define FC_SUBTYPE(fc)		(((fc) >> 4) & 0xF)
#define FC_TO_DS(fc)		((fc) & 0x0100)
#define FC_FROM_DS(fc)		((fc) & 0x0200)
#define FC_MORE_FLAG(fc)	((fc) & 0x0400)
#define FC_RETRY(fc)		((fc) & 0x0800)
#define FC_POWER_MGMT(fc)	((fc) & 0x1000)
#define FC_MORE_DATA(fc)	((fc) & 0x2000)
#define FC_WEP(fc)		((fc) & 0x4000)
#define FC_ORDER(fc)		((fc) & 0x8000)


struct ieee_802_11_mgmt_header {
  u_int16_t fc;
  u_int16_t duration;
  u_int8_t  da[6];
  u_int8_t  sa[6];
  u_int8_t  bssid[6];
  u_int16_t seq_ctrl;
};


struct ieee_802_11_data_header {
	u_int16_t	frame_control;
	u_int16_t	duration;
	u_int8_t	mac1[6];
	u_int8_t	mac2[6];
	u_int8_t	mac3[6];
	u_int16_t	SeqCtl;
	u_int8_t	mac4[6];
//	u_int16_t	gapLen;
//	u_int8_t	gap[8];
};

struct ieee_802_11_control_header {
	u_int16_t	frame_control;
	u_int16_t	duration;
	u_int8_t	mac1[6];
	u_int8_t	mac2[6];
	u_int8_t	mac3[6];
	u_int16_t	SeqCtl;
	u_int8_t	mac4[6];
//	u_int16_t	gapLen;
//	u_int8_t	gap[8];
};

#define CAPABILITY_ESS(cap)     ((cap) & 0x0001)
#define CAPABILITY_IBSS(cap)    ((cap) & 0x0002)
#define CAPABILITY_CFP(cap)     ((cap) & 0x0004)
#define CAPABILITY_CFP_REQ(cap) ((cap) & 0x0008)
#define CAPABILITY_PRIVACY(cap) ((cap) & 0x0010)

struct ssid_t {
  u_int8_t	element_id;
  u_int8_t	length;
  u_char		ssid[33];  /* 32 + 1 for null */
};


struct rates_t {
  u_int8_t	element_id;
  u_int8_t	length;
  u_int8_t	rate[8];
};


struct challenge_t {
  u_int8_t	element_id;
  u_int8_t	length;
  u_int8_t	text[254]; /* 1-253 + 1 for null */
};


struct fh_t {
  u_int8_t	element_id;
  u_int8_t	length;
  u_int16_t	dwell_time;
  u_int8_t	hop_set;
  u_int8_t 	hop_pattern;
  u_int8_t	hop_index;
};


struct ds_t {
  u_int8_t	element_id;
  u_int8_t	length;
  u_int8_t	channel;
};


struct cf_t {
  u_int8_t	element_id;
  u_int8_t	length;
  u_int8_t	count;
  u_int8_t	period;
  u_int16_t	max_duration;
  u_int16_t	dur_remaing;
};


struct tim_t {
  u_int8_t	element_id;
  u_int8_t	length;
  u_int8_t	count;
  u_int8_t	period;
  u_int8_t	bitmap_control;
  u_int8_t	bitmap[251];
};


struct ibss_t {
  u_int8_t	element_id;
  u_int8_t	length;
  u_int16_t	atim_window;
};


#define E_SSID      0
#define E_RATES     1
#define E_FH        2
#define E_DS        3
#define E_CF        4
#define E_TIM       5
#define E_IBSS      6
#define E_CHALLENGE 16
#define E_CISCO     133


struct ieee_802_11_mgmt_body {
  u_int8_t  timestamp[8];
  u_int16_t beacon_interval;
//  u_int16_t 	listen_interval;
//  u_int16_t 	status_code;
//  u_int16_t 	aid;
//  u_char		ap[6];
//  u_int16_t	reason_code;
//  u_int16_t	auth_alg;
//  u_int16_t	auth_trans_seq_num;
//  struct challenge_t  challenge;
  u_int16_t capability_info;
//  struct ssid_t	ssid;
//  struct rates_t 	rates;
//  struct ds_t	ds;
//  struct cf_t	cf;
//  struct fh_t	fh;
//  struct tim_t	tim;
};

/* a 802.11 value */
struct val_80211 {
    unsigned int did;
    unsigned short status, len;
    unsigned int data;
};

/* header attached during prism monitor mode */
struct prism_hdr {
    unsigned int msgcode, msglen;
    char devname[16];
    struct val_80211 hosttime, mactime, channel, rssi, sq, signal,
        noise, rate, istx, frmlen;
};

struct ieee_802_11_data_body {
//FIXME
};

struct ieee_802_11_control_body {
//FIXME
};

struct ctrl_rts_t {
  u_int16_t	fc;
  u_int16_t	duration;
  u_int8_t	ra[6];
  u_int8_t	ta[6];
  u_int8_t	fcs[4];
};

#define CTRL_RTS_LEN	(2+2+6+6+4)

struct ctrl_cts_t {
  u_int16_t	fc;
  u_int16_t	duration;
  u_int8_t	ra[6];
  u_int8_t	fcs[4];
};

#define CTRL_CTS_LEN	(2+2+6+4)

struct ctrl_ack_t {
  u_int16_t	fc;
  u_int16_t	duration;
  u_int8_t	ra[6];
  u_int8_t	fcs[4];
};

#define CTRL_ACK_LEN	(2+2+6+4)

struct ctrl_ps_poll_t {
  u_int16_t	fc;
  u_int16_t	aid;
  u_int8_t	bssid[6];
  u_int8_t	ta[6];
  u_int8_t	fcs[4];
};

#define CTRL_PS_POLL_LEN	(2+2+6+6+4)

struct ctrl_end_t {
  u_int16_t	fc;
  u_int16_t	duration;
  u_int8_t	ra[6];
  u_int8_t	bssid[6];
  u_int8_t	fcs[4];
};

#define CTRL_END_LEN	(2+2+6+6+4)

struct ctrl_end_ack_t {
  u_int16_t	fc;
  u_int16_t	duration;
  u_int8_t	ra[6];
  u_int8_t	bssid[6];
  u_int8_t	fcs[4];
};

#define CTRL_END_ACK_LEN	(2+2+6+6+4)

#define IV_IV(iv)	((iv) & 0xFFFFFF)
#define IV_PAD(iv)	(((iv) >> 24) & 0x3F)
#define IV_KEYID(iv)	(((iv) >> 30) & 0x03)

#endif
