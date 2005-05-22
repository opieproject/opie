#ifndef _LINUX_PCMCIA_H
#define _LINUX_PCMCIA_H

// condensed pcmcia card services header file

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <sys/types.h>
#endif

#ifdef __arm__
typedef u_int   ioaddr_t;
#else
typedef u_short ioaddr_t;
#endif

typedef u_short socket_t;
typedef u_int   event_t;
typedef u_char  cisdata_t;
typedef u_short page_t;

struct client_t;
typedef struct client_t *client_handle_t;

struct window_t;
typedef struct window_t *window_handle_t;

struct region_t;
typedef struct region_t *memory_handle_t;

struct eraseq_t;
typedef struct eraseq_t *eraseq_handle_t;

#ifndef DEV_NAME_LEN
#define DEV_NAME_LEN 32
#endif

typedef char dev_info_t[DEV_NAME_LEN];

#define CISTPL_NULL     0x00
#define CISTPL_DEVICE       0x01
#define CISTPL_LONGLINK_CB  0x02
#define CISTPL_INDIRECT     0x03
#define CISTPL_CONFIG_CB    0x04
#define CISTPL_CFTABLE_ENTRY_CB 0x05
#define CISTPL_LONGLINK_MFC 0x06
#define CISTPL_BAR      0x07
#define CISTPL_PWR_MGMNT    0x08
#define CISTPL_EXTDEVICE    0x09
#define CISTPL_CHECKSUM     0x10
#define CISTPL_LONGLINK_A   0x11
#define CISTPL_LONGLINK_C   0x12
#define CISTPL_LINKTARGET   0x13
#define CISTPL_NO_LINK      0x14
#define CISTPL_VERS_1       0x15
#define CISTPL_ALTSTR       0x16
#define CISTPL_DEVICE_A     0x17
#define CISTPL_JEDEC_C      0x18
#define CISTPL_JEDEC_A      0x19
#define CISTPL_CONFIG       0x1a
#define CISTPL_CFTABLE_ENTRY    0x1b
#define CISTPL_DEVICE_OC    0x1c
#define CISTPL_DEVICE_OA    0x1d
#define CISTPL_DEVICE_GEO   0x1e
#define CISTPL_DEVICE_GEO_A 0x1f
#define CISTPL_MANFID       0x20
#define CISTPL_FUNCID       0x21
#define CISTPL_FUNCE        0x22
#define CISTPL_SWIL     0x23
#define CISTPL_END      0xff
/* Layer 2 tuples */
#define CISTPL_VERS_2       0x40
#define CISTPL_FORMAT       0x41
#define CISTPL_GEOMETRY     0x42
#define CISTPL_BYTEORDER    0x43
#define CISTPL_DATE     0x44
#define CISTPL_BATTERY      0x45
#define CISTPL_FORMAT_A     0x47
/* Layer 3 tuples */
#define CISTPL_ORG      0x46
#define CISTPL_SPCL     0x90

typedef struct cistpl_longlink_t {
    u_int   addr;
} cistpl_longlink_t;

typedef struct cistpl_checksum_t {
    u_short addr;
    u_short len;
    u_char  sum;
} cistpl_checksum_t;

#define CISTPL_MAX_FUNCTIONS    8
#define CISTPL_MFC_ATTR     0x00
#define CISTPL_MFC_COMMON   0x01

typedef struct cistpl_longlink_mfc_t {
    u_char  nfn;
    struct {
    u_char  space;
    u_int   addr;
    } fn[CISTPL_MAX_FUNCTIONS];
} cistpl_longlink_mfc_t;

#define CISTPL_MAX_ALTSTR_STRINGS   4

typedef struct cistpl_altstr_t {
    u_char  ns;
    u_char  ofs[CISTPL_MAX_ALTSTR_STRINGS];
    char    str[254];
} cistpl_altstr_t;

#define CISTPL_DTYPE_NULL   0x00
#define CISTPL_DTYPE_ROM    0x01
#define CISTPL_DTYPE_OTPROM 0x02
#define CISTPL_DTYPE_EPROM  0x03
#define CISTPL_DTYPE_EEPROM 0x04
#define CISTPL_DTYPE_FLASH  0x05
#define CISTPL_DTYPE_SRAM   0x06
#define CISTPL_DTYPE_DRAM   0x07
#define CISTPL_DTYPE_FUNCSPEC   0x0d
#define CISTPL_DTYPE_EXTEND 0x0e

#define CISTPL_MAX_DEVICES  4

typedef struct cistpl_device_t {
    u_char  ndev;
    struct {
    u_char  type;
    u_char  wp;
    u_int   speed;
    u_int   size;
    } dev[CISTPL_MAX_DEVICES];
} cistpl_device_t;

#define CISTPL_DEVICE_MWAIT 0x01
#define CISTPL_DEVICE_3VCC  0x02

typedef struct cistpl_device_o_t {
    u_char      flags;
    cistpl_device_t device;
} cistpl_device_o_t;

#define CISTPL_VERS_1_MAX_PROD_STRINGS  4

typedef struct cistpl_vers_1_t {
    u_char  major;
    u_char  minor;
    u_char  ns;
    u_char  ofs[CISTPL_VERS_1_MAX_PROD_STRINGS];
    char    str[254];
} cistpl_vers_1_t;

typedef struct cistpl_jedec_t {
    u_char  nid;
    struct {
    u_char  mfr;
    u_char  info;
    } id[CISTPL_MAX_DEVICES];
} cistpl_jedec_t;

typedef struct cistpl_manfid_t {
    u_short manf;
    u_short card;
} cistpl_manfid_t;

#define CISTPL_FUNCID_MULTI 0x00
#define CISTPL_FUNCID_MEMORY    0x01
#define CISTPL_FUNCID_SERIAL    0x02
#define CISTPL_FUNCID_PARALLEL  0x03
#define CISTPL_FUNCID_FIXED 0x04
#define CISTPL_FUNCID_VIDEO 0x05
#define CISTPL_FUNCID_NETWORK   0x06
#define CISTPL_FUNCID_AIMS  0x07
#define CISTPL_FUNCID_SCSI  0x08

#define CISTPL_SYSINIT_POST 0x01
#define CISTPL_SYSINIT_ROM  0x02

typedef struct cistpl_funcid_t {
    u_char  func;
    u_char  sysinit;
} cistpl_funcid_t;

typedef struct cistpl_funce_t {
    u_char  type;
    u_char  data[0];
} cistpl_funce_t;

/*======================================================================

    Modem Function Extension Tuples

======================================================================*/

#define CISTPL_FUNCE_SERIAL_IF      0x00
#define CISTPL_FUNCE_SERIAL_CAP     0x01
#define CISTPL_FUNCE_SERIAL_SERV_DATA   0x02
#define CISTPL_FUNCE_SERIAL_SERV_FAX    0x03
#define CISTPL_FUNCE_SERIAL_SERV_VOICE  0x04
#define CISTPL_FUNCE_SERIAL_CAP_DATA    0x05
#define CISTPL_FUNCE_SERIAL_CAP_FAX 0x06
#define CISTPL_FUNCE_SERIAL_CAP_VOICE   0x07
#define CISTPL_FUNCE_SERIAL_IF_DATA 0x08
#define CISTPL_FUNCE_SERIAL_IF_FAX  0x09
#define CISTPL_FUNCE_SERIAL_IF_VOICE    0x0a

/* UART identification */
#define CISTPL_SERIAL_UART_8250     0x00
#define CISTPL_SERIAL_UART_16450    0x01
#define CISTPL_SERIAL_UART_16550    0x02
#define CISTPL_SERIAL_UART_8251     0x03
#define CISTPL_SERIAL_UART_8530     0x04
#define CISTPL_SERIAL_UART_85230    0x05

/* UART capabilities */
#define CISTPL_SERIAL_UART_SPACE    0x01
#define CISTPL_SERIAL_UART_MARK     0x02
#define CISTPL_SERIAL_UART_ODD      0x04
#define CISTPL_SERIAL_UART_EVEN     0x08
#define CISTPL_SERIAL_UART_5BIT     0x01
#define CISTPL_SERIAL_UART_6BIT     0x02
#define CISTPL_SERIAL_UART_7BIT     0x04
#define CISTPL_SERIAL_UART_8BIT     0x08
#define CISTPL_SERIAL_UART_1STOP    0x10
#define CISTPL_SERIAL_UART_MSTOP    0x20
#define CISTPL_SERIAL_UART_2STOP    0x40

typedef struct cistpl_serial_t {
    u_char  uart_type;
    u_char  uart_cap_0;
    u_char  uart_cap_1;
} cistpl_serial_t;

typedef struct cistpl_modem_cap_t {
    u_char  flow;
    u_char  cmd_buf;
    u_char  rcv_buf_0, rcv_buf_1, rcv_buf_2;
    u_char  xmit_buf_0, xmit_buf_1, xmit_buf_2;
} cistpl_modem_cap_t;

#define CISTPL_SERIAL_MOD_103       0x01
#define CISTPL_SERIAL_MOD_V21       0x02
#define CISTPL_SERIAL_MOD_V23       0x04
#define CISTPL_SERIAL_MOD_V22       0x08
#define CISTPL_SERIAL_MOD_212A      0x10
#define CISTPL_SERIAL_MOD_V22BIS    0x20
#define CISTPL_SERIAL_MOD_V26       0x40
#define CISTPL_SERIAL_MOD_V26BIS    0x80
#define CISTPL_SERIAL_MOD_V27BIS    0x01
#define CISTPL_SERIAL_MOD_V29       0x02
#define CISTPL_SERIAL_MOD_V32       0x04
#define CISTPL_SERIAL_MOD_V32BIS    0x08
#define CISTPL_SERIAL_MOD_V34       0x10

#define CISTPL_SERIAL_ERR_MNP2_4    0x01
#define CISTPL_SERIAL_ERR_V42_LAPM  0x02

#define CISTPL_SERIAL_CMPR_V42BIS   0x01
#define CISTPL_SERIAL_CMPR_MNP5     0x02

#define CISTPL_SERIAL_CMD_AT1       0x01
#define CISTPL_SERIAL_CMD_AT2       0x02
#define CISTPL_SERIAL_CMD_AT3       0x04
#define CISTPL_SERIAL_CMD_MNP_AT    0x08
#define CISTPL_SERIAL_CMD_V25BIS    0x10
#define CISTPL_SERIAL_CMD_V25A      0x20
#define CISTPL_SERIAL_CMD_DMCL      0x40

typedef struct cistpl_data_serv_t {
    u_char  max_data_0;
    u_char  max_data_1;
    u_char  modulation_0;
    u_char  modulation_1;
    u_char  error_control;
    u_char  compression;
    u_char  cmd_protocol;
    u_char  escape;
    u_char  encrypt;
    u_char  misc_features;
    u_char  ccitt_code[0];
} cistpl_data_serv_t;

typedef struct cistpl_fax_serv_t {
    u_char  max_data_0;
    u_char  max_data_1;
    u_char  modulation;
    u_char  encrypt;
    u_char  features_0;
    u_char  features_1;
    u_char  ccitt_code[0];
} cistpl_fax_serv_t;

typedef struct cistpl_voice_serv_t {
    u_char  max_data_0;
    u_char  max_data_1;
} cistpl_voice_serv_t;

/*======================================================================

    LAN Function Extension Tuples

======================================================================*/

#define CISTPL_FUNCE_LAN_TECH       0x01
#define CISTPL_FUNCE_LAN_SPEED      0x02
#define CISTPL_FUNCE_LAN_MEDIA      0x03
#define CISTPL_FUNCE_LAN_NODE_ID    0x04
#define CISTPL_FUNCE_LAN_CONNECTOR  0x05

/* LAN technologies */
#define CISTPL_LAN_TECH_ARCNET      0x01
#define CISTPL_LAN_TECH_ETHERNET    0x02
#define CISTPL_LAN_TECH_TOKENRING   0x03
#define CISTPL_LAN_TECH_LOCALTALK   0x04
#define CISTPL_LAN_TECH_FDDI        0x05
#define CISTPL_LAN_TECH_ATM     0x06
#define CISTPL_LAN_TECH_WIRELESS    0x07

typedef struct cistpl_lan_tech_t {
    u_char  tech;
} cistpl_lan_tech_t;

typedef struct cistpl_lan_speed_t {
    u_int   speed;
} cistpl_lan_speed_t;

/* LAN media definitions */
#define CISTPL_LAN_MEDIA_UTP        0x01
#define CISTPL_LAN_MEDIA_STP        0x02
#define CISTPL_LAN_MEDIA_THIN_COAX  0x03
#define CISTPL_LAN_MEDIA_THICK_COAX 0x04
#define CISTPL_LAN_MEDIA_FIBER      0x05
#define CISTPL_LAN_MEDIA_900MHZ     0x06
#define CISTPL_LAN_MEDIA_2GHZ       0x07
#define CISTPL_LAN_MEDIA_5GHZ       0x08
#define CISTPL_LAN_MEDIA_DIFF_IR    0x09
#define CISTPL_LAN_MEDIA_PTP_IR     0x0a

typedef struct cistpl_lan_media_t {
    u_char  media;
} cistpl_lan_media_t;

typedef struct cistpl_lan_node_id_t {
    u_char  nb;
    u_char  id[16];
} cistpl_lan_node_id_t;

typedef struct cistpl_lan_connector_t {
    u_char  code;
} cistpl_lan_connector_t;

/*======================================================================

    IDE Function Extension Tuples

======================================================================*/

#define CISTPL_IDE_INTERFACE        0x01

typedef struct cistpl_ide_interface_t {
    u_char  interface;
} cistpl_ide_interface_t;

/* First feature byte */
#define CISTPL_IDE_SILICON      0x04
#define CISTPL_IDE_UNIQUE       0x08
#define CISTPL_IDE_DUAL         0x10

/* Second feature byte */
#define CISTPL_IDE_HAS_SLEEP        0x01
#define CISTPL_IDE_HAS_STANDBY      0x02
#define CISTPL_IDE_HAS_IDLE     0x04
#define CISTPL_IDE_LOW_POWER        0x08
#define CISTPL_IDE_REG_INHIBIT      0x10
#define CISTPL_IDE_HAS_INDEX        0x20
#define CISTPL_IDE_IOIS16       0x40

typedef struct cistpl_ide_feature_t {
    u_char  feature1;
    u_char  feature2;
} cistpl_ide_feature_t;

#define CISTPL_FUNCE_IDE_IFACE      0x01
#define CISTPL_FUNCE_IDE_MASTER     0x02
#define CISTPL_FUNCE_IDE_SLAVE      0x03

/*======================================================================

    Configuration Table Entries

======================================================================*/

#define CISTPL_BAR_SPACE    0x07
#define CISTPL_BAR_SPACE_IO 0x10
#define CISTPL_BAR_PREFETCH 0x20
#define CISTPL_BAR_CACHEABLE    0x40
#define CISTPL_BAR_1MEG_MAP 0x80

typedef struct cistpl_bar_t {
    u_char  attr;
    u_int   size;
} cistpl_bar_t;

typedef struct cistpl_config_t {
    u_char  last_idx;
    u_int   base;
    u_int   rmask[4];
    u_char  subtuples;
} cistpl_config_t;

/* These are bits in the 'present' field, and indices in 'param' */
#define CISTPL_POWER_VNOM   0
#define CISTPL_POWER_VMIN   1
#define CISTPL_POWER_VMAX   2
#define CISTPL_POWER_ISTATIC    3
#define CISTPL_POWER_IAVG   4
#define CISTPL_POWER_IPEAK  5
#define CISTPL_POWER_IDOWN  6

#define CISTPL_POWER_HIGHZ_OK   0x01
#define CISTPL_POWER_HIGHZ_REQ  0x02

typedef struct cistpl_power_t {
    u_char  present;
    u_char  flags;
    u_int   param[7];
} cistpl_power_t;

typedef struct cistpl_timing_t {
    u_int   wait, waitscale;
    u_int   ready, rdyscale;
    u_int   reserved, rsvscale;
} cistpl_timing_t;

#define CISTPL_IO_LINES_MASK    0x1f
#define CISTPL_IO_8BIT      0x20
#define CISTPL_IO_16BIT     0x40
#define CISTPL_IO_RANGE     0x80

#define CISTPL_IO_MAX_WIN   16

typedef struct cistpl_io_t {
    u_char  flags;
    u_char  nwin;
    struct {
    u_int   base;
    u_int   len;
    } win[CISTPL_IO_MAX_WIN];
} cistpl_io_t;

typedef struct cistpl_irq_t {
    u_int   IRQInfo1;
    u_int   IRQInfo2;
} cistpl_irq_t;

#define CISTPL_MEM_MAX_WIN  8

typedef struct cistpl_mem_t {
    u_char  flags;
    u_char  nwin;
    struct {
    u_int   len;
    u_int   card_addr;
    u_int   host_addr;
    } win[CISTPL_MEM_MAX_WIN];
} cistpl_mem_t;

#define CISTPL_CFTABLE_DEFAULT      0x0001
#define CISTPL_CFTABLE_BVDS     0x0002
#define CISTPL_CFTABLE_WP       0x0004
#define CISTPL_CFTABLE_RDYBSY       0x0008
#define CISTPL_CFTABLE_MWAIT        0x0010
#define CISTPL_CFTABLE_AUDIO        0x0800
#define CISTPL_CFTABLE_READONLY     0x1000
#define CISTPL_CFTABLE_PWRDOWN      0x2000

typedef struct cistpl_cftable_entry_t {
    u_char      index;
    u_short     flags;
    u_char      interface;
    cistpl_power_t  vcc, vpp1, vpp2;
    cistpl_timing_t timing;
    cistpl_io_t     io;
    cistpl_irq_t    irq;
    cistpl_mem_t    mem;
    u_char      subtuples;
} cistpl_cftable_entry_t;

#define CISTPL_CFTABLE_MASTER       0x000100
#define CISTPL_CFTABLE_INVALIDATE   0x000200
#define CISTPL_CFTABLE_VGA_PALETTE  0x000400
#define CISTPL_CFTABLE_PARITY       0x000800
#define CISTPL_CFTABLE_WAIT     0x001000
#define CISTPL_CFTABLE_SERR     0x002000
#define CISTPL_CFTABLE_FAST_BACK    0x004000
#define CISTPL_CFTABLE_BINARY_AUDIO 0x010000
#define CISTPL_CFTABLE_PWM_AUDIO    0x020000

typedef struct cistpl_cftable_entry_cb_t {
    u_char      index;
    u_int       flags;
    cistpl_power_t  vcc, vpp1, vpp2;
    u_char      io;
    cistpl_irq_t    irq;
    u_char      mem;
    u_char      subtuples;
} cistpl_cftable_entry_cb_t;

typedef struct cistpl_device_geo_t {
    u_char      ngeo;
    struct {
    u_char      buswidth;
    u_int       erase_block;
    u_int       read_block;
    u_int       write_block;
    u_int       partition;
    u_int       interleave;
    } geo[CISTPL_MAX_DEVICES];
} cistpl_device_geo_t;

typedef struct cistpl_vers_2_t {
    u_char  vers;
    u_char  comply;
    u_short dindex;
    u_char  vspec8, vspec9;
    u_char  nhdr;
    u_char  vendor, info;
    char    str[244];
} cistpl_vers_2_t;

typedef struct cistpl_org_t {
    u_char  data_org;
    char    desc[30];
} cistpl_org_t;

#define CISTPL_ORG_FS       0x00
#define CISTPL_ORG_APPSPEC  0x01
#define CISTPL_ORG_XIP      0x02

typedef struct cistpl_format_t {
    u_char  type;
    u_char  edc;
    u_int   offset;
    u_int   length;
} cistpl_format_t;

#define CISTPL_FORMAT_DISK  0x00
#define CISTPL_FORMAT_MEM   0x01

#define CISTPL_EDC_NONE     0x00
#define CISTPL_EDC_CKSUM    0x01
#define CISTPL_EDC_CRC      0x02
#define CISTPL_EDC_PCC      0x03

typedef union cisparse_t {
    cistpl_device_t     device;
    cistpl_checksum_t       checksum;
    cistpl_longlink_t       longlink;
    cistpl_longlink_mfc_t   longlink_mfc;
    cistpl_vers_1_t     version_1;
    cistpl_altstr_t     altstr;
    cistpl_jedec_t      jedec;
    cistpl_manfid_t     manfid;
    cistpl_funcid_t     funcid;
    cistpl_funce_t      funce;
    cistpl_bar_t        bar;
    cistpl_config_t     config;
    cistpl_cftable_entry_t  cftable_entry;
    cistpl_cftable_entry_cb_t   cftable_entry_cb;
    cistpl_device_geo_t     device_geo;
    cistpl_vers_2_t     vers_2;
    cistpl_org_t        org;
    cistpl_format_t     format;
} cisparse_t;

typedef struct tuple_t {
    u_int   Attributes;
    cisdata_t   DesiredTuple;
    u_int   Flags;      /* internal use */
    u_int   LinkOffset; /* internal use */
    u_int   CISOffset;  /* internal use */
    cisdata_t   TupleCode;
    cisdata_t   TupleLink;
    cisdata_t   TupleOffset;
    cisdata_t   TupleDataMax;
    cisdata_t   TupleDataLen;
    cisdata_t   *TupleData;
} tuple_t;

/* Special cisdata_t value */
#define RETURN_FIRST_TUPLE  0xff

/* Attributes for tuple calls */
#define TUPLE_RETURN_LINK   0x01
#define TUPLE_RETURN_COMMON 0x02

/* For ValidateCIS */
typedef struct cisinfo_t {
    u_int   Chains;
} cisinfo_t;

#define CISTPL_MAX_CIS_SIZE 0x200

/* For ReplaceCIS */
typedef struct cisdump_t {
    u_int   Length;
    cisdata_t   Data[CISTPL_MAX_CIS_SIZE];
} cisdump_t;

#endif /* LINUX_CISTPL_H */

/* For AccessConfigurationRegister */
typedef struct conf_reg_t {
    u_char  Function;
    u_int   Action;
    off_t   Offset;
    u_int   Value;
} conf_reg_t;

/* Actions */
#define CS_READ     1
#define CS_WRITE    2

/* for AdjustResourceInfo */
typedef struct adjust_t {
    u_int   Action;
    u_int   Resource;
    u_int   Attributes;
    union {
    struct memory {
        u_long  Base;
        u_long  Size;
    } memory;
    struct io {
        ioaddr_t    BasePort;
        ioaddr_t    NumPorts;
        u_int   IOAddrLines;
    } io;
    struct irq {
        u_int   IRQ;
    } irq;
    } resource;
} adjust_t;

/* Action field */
#define REMOVE_MANAGED_RESOURCE     1
#define ADD_MANAGED_RESOURCE        2
#define GET_FIRST_MANAGED_RESOURCE  3
#define GET_NEXT_MANAGED_RESOURCE   4
/* Resource field */
#define RES_MEMORY_RANGE        1
#define RES_IO_RANGE            2
#define RES_IRQ             3
/* Attribute field */
#define RES_IRQ_TYPE            0x03
#define RES_IRQ_TYPE_EXCLUSIVE      0
#define RES_IRQ_TYPE_TIME       1
#define RES_IRQ_TYPE_DYNAMIC        2
#define RES_IRQ_CSC         0x04
#define RES_SHARED          0x08
#define RES_RESERVED            0x10
#define RES_ALLOCATED           0x20
#define RES_REMOVED         0x40

typedef struct servinfo_t {
    char    Signature[2];
    u_int   Count;
    u_int   Revision;
    u_int   CSLevel;
    char    *VendorString;
} servinfo_t;

typedef struct event_callback_args_t {
    client_handle_t client_handle;
    void    *info;
    void    *mtdrequest;
    void    *buffer;
    void    *misc;
    void    *client_data;
    struct bus_operations *bus;
} event_callback_args_t;

/* for GetConfigurationInfo */
typedef struct config_info_t {
    u_char  Function;
    u_int   Attributes;
    u_int   Vcc, Vpp1, Vpp2;
    u_int   IntType;
    u_int   ConfigBase;
    u_char  Status, Pin, Copy, Option, ExtStatus;
    u_int   Present;
    u_int   CardValues;
    u_int   AssignedIRQ;
    u_int   IRQAttributes;
    ioaddr_t    BasePort1;
    ioaddr_t    NumPorts1;
    u_int   Attributes1;
    ioaddr_t    BasePort2;
    ioaddr_t    NumPorts2;
    u_int   Attributes2;
    u_int   IOAddrLines;
} config_info_t;

/* For CardValues field */
#define CV_OPTION_VALUE     0x01
#define CV_STATUS_VALUE     0x02
#define CV_PIN_REPLACEMENT  0x04
#define CV_COPY_VALUE       0x08
#define CV_EXT_STATUS       0x10

/* For GetFirst/NextClient */
typedef struct client_req_t {
    socket_t    Socket;
    u_int   Attributes;
} client_req_t;

#define CLIENT_THIS_SOCKET  0x01

/* For RegisterClient */
typedef struct client_reg_t {
    dev_info_t  *dev_info;
    u_int   Attributes;
    u_int   EventMask;
    int     (*event_handler)(event_t event, int priority,
                 event_callback_args_t *);
    event_callback_args_t event_callback_args;
    u_int   Version;
} client_reg_t;

/* ModifyConfiguration */
typedef struct modconf_t {
    u_int   Attributes;
    u_int   Vcc, Vpp1, Vpp2;
} modconf_t;

/* Attributes for ModifyConfiguration */
#define CONF_IRQ_CHANGE_VALID   0x100
#define CONF_VCC_CHANGE_VALID   0x200
#define CONF_VPP1_CHANGE_VALID  0x400
#define CONF_VPP2_CHANGE_VALID  0x800

/* For RequestConfiguration */
typedef struct config_req_t {
    u_int   Attributes;
    u_int   Vcc, Vpp1, Vpp2;
    u_int   IntType;
    u_int   ConfigBase;
    u_char  Status, Pin, Copy, ExtStatus;
    u_char  ConfigIndex;
    u_int   Present;
} config_req_t;

/* Attributes for RequestConfiguration */
#define CONF_ENABLE_IRQ     0x01
#define CONF_ENABLE_DMA     0x02
#define CONF_ENABLE_SPKR    0x04
#define CONF_VALID_CLIENT   0x100

/* IntType field */
#define INT_MEMORY      0x01
#define INT_MEMORY_AND_IO   0x02
#define INT_CARDBUS     0x04
#define INT_ZOOMED_VIDEO    0x08

/* For RequestIO and ReleaseIO */
typedef struct io_req_t {
    ioaddr_t    BasePort1;
    ioaddr_t    NumPorts1;
    u_int   Attributes1;
    ioaddr_t    BasePort2;
    ioaddr_t    NumPorts2;
    u_int   Attributes2;
    u_int   IOAddrLines;
} io_req_t;

/* Attributes for RequestIO and ReleaseIO */
#define IO_SHARED       0x01
#define IO_FIRST_SHARED     0x02
#define IO_FORCE_ALIAS_ACCESS   0x04
#define IO_DATA_PATH_WIDTH  0x18
#define IO_DATA_PATH_WIDTH_8    0x00
#define IO_DATA_PATH_WIDTH_16   0x08
#define IO_DATA_PATH_WIDTH_AUTO 0x10

/* For RequestIRQ and ReleaseIRQ */
typedef struct irq_req_t {
    u_int   Attributes;
    u_int   AssignedIRQ;
    u_int   IRQInfo1, IRQInfo2;
    void    *Handler;
    void    *Instance;
} irq_req_t;

/* Attributes for RequestIRQ and ReleaseIRQ */
#define IRQ_TYPE            0x03
#define IRQ_TYPE_EXCLUSIVE      0x00
#define IRQ_TYPE_TIME           0x01
#define IRQ_TYPE_DYNAMIC_SHARING    0x02
#define IRQ_FORCED_PULSE        0x04
#define IRQ_FIRST_SHARED        0x08
#define IRQ_HANDLE_PRESENT      0x10
#define IRQ_PULSE_ALLOCATED     0x100

/* Bits in IRQInfo1 field */
#define IRQ_MASK        0x0f
#define IRQ_NMI_ID      0x01
#define IRQ_IOCK_ID     0x02
#define IRQ_BERR_ID     0x04
#define IRQ_VEND_ID     0x08
#define IRQ_INFO2_VALID     0x10
#define IRQ_LEVEL_ID        0x20
#define IRQ_PULSE_ID        0x40
#define IRQ_SHARE_ID        0x80

typedef struct eventmask_t {
    u_int   Attributes;
    u_int   EventMask;
} eventmask_t;

#define CONF_EVENT_MASK_VALID   0x01

/* Configuration registers present */
#define PRESENT_OPTION      0x001
#define PRESENT_STATUS      0x002
#define PRESENT_PIN_REPLACE 0x004
#define PRESENT_COPY        0x008
#define PRESENT_EXT_STATUS  0x010
#define PRESENT_IOBASE_0    0x020
#define PRESENT_IOBASE_1    0x040
#define PRESENT_IOBASE_2    0x080
#define PRESENT_IOBASE_3    0x100
#define PRESENT_IOSIZE      0x200

/* For GetMemPage, MapMemPage */
typedef struct memreq_t {
    u_int   CardOffset;
    page_t  Page;
} memreq_t;

/* For ModifyWindow */
typedef struct modwin_t {
    u_int   Attributes;
    u_int   AccessSpeed;
} modwin_t;

/* For RequestWindow */
typedef struct win_req_t {
    u_int   Attributes;
    u_long  Base;
    u_int   Size;
    u_int   AccessSpeed;
} win_req_t;

/* Attributes for RequestWindow */
#define WIN_ADDR_SPACE      0x0001
#define WIN_ADDR_SPACE_MEM  0x0000
#define WIN_ADDR_SPACE_IO   0x0001
#define WIN_MEMORY_TYPE     0x0002
#define WIN_MEMORY_TYPE_CM  0x0000
#define WIN_MEMORY_TYPE_AM  0x0002
#define WIN_ENABLE      0x0004
#define WIN_DATA_WIDTH      0x0018
#define WIN_DATA_WIDTH_8    0x0000
#define WIN_DATA_WIDTH_16   0x0008
#define WIN_DATA_WIDTH_32   0x0010
#define WIN_PAGED       0x0020
#define WIN_SHARED      0x0040
#define WIN_FIRST_SHARED    0x0080
#define WIN_USE_WAIT        0x0100
#define WIN_STRICT_ALIGN    0x0200
#define WIN_MAP_BELOW_1MB   0x0400
#define WIN_PREFETCH        0x0800
#define WIN_CACHEABLE       0x1000
#define WIN_BAR_MASK        0xe000
#define WIN_BAR_SHIFT       13

/* Attributes for RegisterClient */
#define INFO_MASTER_CLIENT  0x01
#define INFO_IO_CLIENT      0x02
#define INFO_MTD_CLIENT     0x04
#define INFO_MEM_CLIENT     0x08
#define MAX_NUM_CLIENTS     3

#define INFO_CARD_SHARE     0x10
#define INFO_CARD_EXCL      0x20

typedef struct cs_status_t {
    u_char  Function;
    event_t     CardState;
    event_t SocketState;
} cs_status_t;

typedef struct error_info_t {
    int     func;
    int     retcode;
} error_info_t;

/* Special stuff for binding drivers to sockets */
typedef struct bind_req_t {
    socket_t    Socket;
    u_char  Function;
    dev_info_t  *dev_info;
} bind_req_t;

/* Flag to bind to all functions */
#define BIND_FN_ALL 0xff

typedef struct mtd_bind_t {
    socket_t    Socket;
    u_int   Attributes;
    u_int   CardOffset;
    dev_info_t  *dev_info;
} mtd_bind_t;

/* For GetFirstRegion and GetNextRegion */
typedef struct region_info_t {
    u_int               Attributes;
    u_int               CardOffset;
    u_int               RegionSize;
    u_int               AccessSpeed;
    u_int               BlockSize;
    u_int               PartMultiple;
    u_char              JedecMfr, JedecInfo;
    memory_handle_t     next;
} region_info_t;

#define REGION_TYPE             0x0001
#define REGION_TYPE_CM          0x0000
#define REGION_TYPE_AM          0x0001
#define REGION_PREFETCH         0x0008
#define REGION_CACHEABLE        0x0010
#define REGION_BAR_MASK         0xe000
#define REGION_BAR_SHIFT        13

/* For OpenMemory */
typedef struct open_mem_t {
    u_int               Attributes;
    u_int               Offset;
} open_mem_t;

/* Attributes for OpenMemory */
#define MEMORY_TYPE             0x0001
#define MEMORY_TYPE_CM          0x0000
#define MEMORY_TYPE_AM          0x0001
#define MEMORY_EXCLUSIVE        0x0002
#define MEMORY_PREFETCH         0x0008
#define MEMORY_CACHEABLE        0x0010
#define MEMORY_BAR_MASK         0xe000
#define MEMORY_BAR_SHIFT        13

typedef struct eraseq_entry_t {
    memory_handle_t     Handle;
    u_char              State;
    u_int               Size;
    u_int               Offset;
    void                *Optional;
} eraseq_entry_t;

typedef struct eraseq_hdr_t {
    int                 QueueEntryCnt;
    eraseq_entry_t      *QueueEntryArray;
} eraseq_hdr_t;

/* Events */
#define CS_EVENT_PRI_LOW        0
#define CS_EVENT_PRI_HIGH       1

#define CS_EVENT_WRITE_PROTECT      0x000001
#define CS_EVENT_CARD_LOCK      0x000002
#define CS_EVENT_CARD_INSERTION     0x000004
#define CS_EVENT_CARD_REMOVAL       0x000008
#define CS_EVENT_BATTERY_DEAD       0x000010
#define CS_EVENT_BATTERY_LOW        0x000020
#define CS_EVENT_READY_CHANGE       0x000040
#define CS_EVENT_CARD_DETECT        0x000080
#define CS_EVENT_RESET_REQUEST      0x000100
#define CS_EVENT_RESET_PHYSICAL     0x000200
#define CS_EVENT_CARD_RESET     0x000400
#define CS_EVENT_REGISTRATION_COMPLETE  0x000800
#define CS_EVENT_RESET_COMPLETE     0x001000
#define CS_EVENT_PM_SUSPEND     0x002000
#define CS_EVENT_PM_RESUME      0x004000
#define CS_EVENT_INSERTION_REQUEST  0x008000
#define CS_EVENT_EJECTION_REQUEST   0x010000
#define CS_EVENT_MTD_REQUEST        0x020000
#define CS_EVENT_ERASE_COMPLETE     0x040000
#define CS_EVENT_REQUEST_ATTENTION  0x080000
#define CS_EVENT_CB_DETECT      0x100000
#define CS_EVENT_3VCARD         0x200000
#define CS_EVENT_XVCARD         0x400000

/* Return codes */
#define CS_SUCCESS      0x00
#define CS_BAD_ADAPTER      0x01
#define CS_BAD_ATTRIBUTE    0x02
#define CS_BAD_BASE     0x03
#define CS_BAD_EDC      0x04
#define CS_BAD_IRQ      0x06
#define CS_BAD_OFFSET       0x07
#define CS_BAD_PAGE     0x08
#define CS_READ_FAILURE     0x09
#define CS_BAD_SIZE     0x0a
#define CS_BAD_SOCKET       0x0b
#define CS_BAD_TYPE     0x0d
#define CS_BAD_VCC      0x0e
#define CS_BAD_VPP      0x0f
#define CS_BAD_WINDOW       0x11
#define CS_WRITE_FAILURE    0x12
#define CS_NO_CARD      0x14
#define CS_UNSUPPORTED_FUNCTION 0x15
#define CS_UNSUPPORTED_MODE 0x16
#define CS_BAD_SPEED        0x17
#define CS_BUSY         0x18
#define CS_GENERAL_FAILURE  0x19
#define CS_WRITE_PROTECTED  0x1a
#define CS_BAD_ARG_LENGTH   0x1b
#define CS_BAD_ARGS     0x1c
#define CS_CONFIGURATION_LOCKED 0x1d
#define CS_IN_USE       0x1e
#define CS_NO_MORE_ITEMS    0x1f
#define CS_OUT_OF_RESOURCE  0x20
#define CS_BAD_HANDLE       0x21

#define CS_BAD_TUPLE        0x40



typedef struct tuple_parse_t {
    tuple_t		tuple;
    cisdata_t		data[255];
    cisparse_t		parse;
} tuple_parse_t;

typedef struct win_info_t {
    window_handle_t	handle;
    win_req_t		window;
    memreq_t		map;
} win_info_t;

typedef struct bind_info_t {
    dev_info_t		dev_info;
    u_char		function;
    struct dev_link_t	*instance;
    char		name[DEV_NAME_LEN];
    u_short		major, minor;
    void		*next;
} bind_info_t;

typedef struct mtd_info_t {
    dev_info_t		dev_info;
    u_int		Attributes;
    u_int		CardOffset;
} mtd_info_t;

typedef union ds_ioctl_arg_t {
    servinfo_t		servinfo;
    adjust_t		adjust;
    config_info_t	config;
    tuple_t		tuple;
    tuple_parse_t	tuple_parse;
    client_req_t	client_req;
    cs_status_t		status;
    conf_reg_t		conf_reg;
    cisinfo_t		cisinfo;
    region_info_t	region;
    bind_info_t		bind_info;
    mtd_info_t		mtd_info;
    win_info_t		win_info;
    cisdump_t		cisdump;
} ds_ioctl_arg_t;

#define DS_GET_CARD_SERVICES_INFO	_IOR ('d', 1, servinfo_t)
#define DS_ADJUST_RESOURCE_INFO		_IOWR('d', 2, adjust_t)
#define DS_GET_CONFIGURATION_INFO	_IOWR('d', 3, config_info_t)
#define DS_GET_FIRST_TUPLE		_IOWR('d', 4, tuple_t)
#define DS_GET_NEXT_TUPLE		_IOWR('d', 5, tuple_t)
#define DS_GET_TUPLE_DATA		_IOWR('d', 6, tuple_parse_t)
#define DS_PARSE_TUPLE			_IOWR('d', 7, tuple_parse_t)
#define DS_RESET_CARD			_IO  ('d', 8)
#define DS_GET_STATUS			_IOWR('d', 9, cs_status_t)
#define DS_ACCESS_CONFIGURATION_REGISTER _IOWR('d', 10, conf_reg_t)
#define DS_VALIDATE_CIS			_IOR ('d', 11, cisinfo_t)
#define DS_SUSPEND_CARD			_IO  ('d', 12)
#define DS_RESUME_CARD			_IO  ('d', 13)
#define DS_EJECT_CARD			_IO  ('d', 14)
#define DS_INSERT_CARD			_IO  ('d', 15)
#define DS_GET_FIRST_REGION		_IOWR('d', 16, region_info_t)
#define DS_GET_NEXT_REGION		_IOWR('d', 17, region_info_t)
#define DS_REPLACE_CIS			_IOWR('d', 18, cisdump_t)
#define DS_GET_FIRST_WINDOW		_IOR ('d', 19, win_info_t)
#define DS_GET_NEXT_WINDOW		_IOWR('d', 20, win_info_t)
#define DS_GET_MEM_PAGE			_IOWR('d', 21, win_info_t)

#define DS_BIND_REQUEST			_IOWR('d', 60, bind_info_t)
#define DS_GET_DEVICE_INFO		_IOWR('d', 61, bind_info_t)
#define DS_GET_NEXT_DEVICE		_IOWR('d', 62, bind_info_t)
#define DS_UNBIND_REQUEST		_IOW ('d', 63, bind_info_t)
#define DS_BIND_MTD			_IOWR('d', 64, mtd_info_t)
