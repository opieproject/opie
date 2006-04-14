/* $Id: bt-serial.c,v 1.1 2006-04-14 19:22:37 korovkin Exp $
 *	Bluetooth serial forwarder functions implementation
 *
 *	(c) Copyright 2006 GPL
 *
 *	This software is provided under the GNU public license, incorporated
 *	herein by reference. The software is provided without warranty or 
 *	support.
 */
#include "bt-serial.h"
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

static int hserv = -1; //Server socket
sdp_session_t* session = NULL; //session with an SDP server

static sdp_session_t* register_service(uint8_t rfchannel)
{
    int err = 0;
    sdp_profile_desc_t profile[1];
    uint32_t service_uuid_int[] = { 0, 0, 0, 0xABCD };
    const char *service_name = "Serial forwarder";
    const char *service_dsc = "Serial forwarder";
    const char *service_prov = "OPIE team";
    uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, svc_uuid, service_uuid;
    sdp_list_t* l2cap_list = 0;
    sdp_list_t* rfcomm_list = 0;
    sdp_list_t* root_list = 0;
    sdp_list_t* proto_list = 0;
    sdp_list_t* access_proto_list = 0;
    sdp_list_t* profile_list = 0;
    sdp_list_t* service_list = 0;
    sdp_data_t* channel = 0; 
    sdp_record_t* record = sdp_record_alloc();    
    sdp_session_t* lsession = 0;

    // set the general service ID
    sdp_uuid128_create(&svc_uuid, &service_uuid_int);
    sdp_set_service_id(record, svc_uuid);
    // make the service record publicly browsable
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    root_list = sdp_list_append(0, &root_uuid);
    sdp_set_browse_groups( record, root_list );

    // set l2cap information
    sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    l2cap_list = sdp_list_append( 0, &l2cap_uuid );
    proto_list = sdp_list_append( 0, l2cap_list );

    // set rfcomm information
    sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
    channel = sdp_data_alloc(SDP_UINT8, &rfchannel);
    rfcomm_list = sdp_list_append( 0, &rfcomm_uuid );
    sdp_list_append( rfcomm_list, channel );
    sdp_list_append( proto_list, rfcomm_list );

    // attach protocol information to service record
    access_proto_list = sdp_list_append( 0, proto_list );
    sdp_set_access_protos( record, access_proto_list );

    sdp_uuid16_create(&service_uuid, SERIAL_PORT_SVCLASS_ID);
    service_list = sdp_list_append( 0, &service_uuid );
    sdp_set_service_classes(record, service_list);

    profile[0].version = 0x0100;
    sdp_uuid16_create(&profile[0].uuid, SERIAL_PORT_PROFILE_ID);
    profile_list = sdp_list_append(0, &profile[0]);
    sdp_set_profile_descs(record, profile_list);
    
    // set the name, provider, and description
    sdp_set_info_attr(record, service_name, service_prov, service_dsc);

    // connect to the local SDP server, register the service record, and 
    // disconnect
    lsession = sdp_connect(BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY);
    if (lsession == NULL)
        goto errout;
    err = sdp_record_register(lsession, record, 0);
    if (err) {
        sdp_close(lsession);
        lsession = NULL;
    }
errout:
    // cleanup
    sdp_data_free( channel );
    sdp_list_free( l2cap_list, 0 );
    sdp_list_free( rfcomm_list, 0 );
    sdp_list_free( root_list, 0 );
    sdp_list_free( access_proto_list, 0 );
    sdp_list_free( profile_list, 0 );
    sdp_list_free( service_list, 0 );
    
    return lsession;
}

/*
 * Function opens and configures serial port
 * portName - name of the serial port
 * return 0 on success, -1 on error
 */
int openSerial(const char* portName)
{
    struct termios tc; //port attributes
    int hserial = -1; //serial port handler
    int result; //function call result
    if ((hserial = open(portName, O_RDWR)) < 0)
        goto errout;
    if ((result = tcgetattr(hserial, &tc)) < 0)
        goto errout;
    cfmakeraw(&tc);
    cfsetispeed(&tc, B9600);
    cfsetospeed(&tc, B9600);
    if ((result = tcsetattr(hserial, TCSANOW, &tc)) < 0)
        goto errout;
    if (result == 0)
        errno = 0;
errout:
    if (errno) {
        if (hserial >= 0) {
            close(hserial);
            hserial = -1;
        }
    }
    return hserial;
}

/*
 * bt_serialStart 
 * Function starts bt-serial service
 * return 0 success -1 on error
 */
int bt_serialStart(void)
{
    struct sockaddr_rc loc_addr; //server address
    int i; //just an index variable
    int result = 0; //function call result
    if (hserv >= 0)
        return 0;
    hserv = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    // bind socket to port 1 of the first available 
    // local bluetooth adapter
    memset(&loc_addr, 0, sizeof(struct sockaddr_rc));
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    for (i = 1; i < 32; i++) {
        loc_addr.rc_channel = (uint8_t)i;
        if (!(result = bind(hserv,
            (struct sockaddr *)&loc_addr, 
            sizeof(loc_addr))) || errno == EINVAL) {
            break;
        }
    }
    if (result != 0)
        goto errout;
    else
        errno = 0;
    if (listen(hserv, 1) < 0)
        goto errout;
    session = register_service(loc_addr.rc_channel);
errout:
    if (errno) {
        result = errno;
        close(hserv);
        hserv = -1;
        if (session != NULL)
            sdp_close(session);
        errno = result;
        result = -1;
    }
    return result;
}

/*
 * bt_serialStop
 * Function stops bt-serial service
 * return device handler on success -1 on error
 */
int bt_serialStop(void)
{
    int result = 0; //Function call result

    if (hserv >= 0) {
        result = close(hserv);
        hserv = -1;
        if (session != NULL)
            sdp_close(session);
    }
    return result;
}

/*
 * btWrite
 * hbt - handler of the BT connection
 * buf - buffer to write
 * plen - total length to write (and zero it!!!)
 * return number of bytes written on success or -1
 */
int btWrite(int hbt, uint8_t* buf, int* plen)
{
    int result; //Function call result
    const suseconds_t writeDelay = 100000L; //wait after writing
    result = write(hbt, buf, *plen);
#ifdef _DEBUG_
    printf("ser->bt %d\n", *plen);
#endif
    *plen = 0;
    usleep(writeDelay);
    return result;
}

/*
 * bt_serialForward
 * Function forwards data received from bt-connection to serial and backward
 * conn - connection handler
 * portName - name of the serial port to open
 * return 0 success -1 on error
 */
/*
 * This function has a hack. My BT adapter hangs if you try to write small
 * portions of data to it to often. That's why we either wait for big enough
 * (> wrThresh) portion of data from a serial port and write it to BT or 
 * wait for a timeout (tv).
 */
int bt_serialForward(BTSerialConn* conn, const char* portName)
{
    int result; //Function call result
    fd_set inSet; //Set we scan for input
    uint8_t inBuf[1500]; //buffer we read and write
    uint8_t outBuf[1500]; //buffer we read and write
    int outBytes; //bytes to be written to bt
    int nbytes = 0; //number of bytes we could read
    int maxfd; //maximal filehandler
    struct timeval tv; //time we shall wait for select
    const int wrThresh = 250; //threshold after which we send packet to bt
    const suseconds_t waitDelay = 200000L; //Time (us) we wait for data 
    struct sockaddr_rc rem_addr; //client address
    int len = sizeof(rem_addr); //argument length

    if (conn == NULL) {
        errno = EINVAL;
        return -1;
    }
    memset(&rem_addr, 0, sizeof(struct sockaddr_rc));
    conn->bt_handler = -1;
    conn->ser_handler = -1;
    conn->bt_handler = accept(hserv, (struct sockaddr *)&rem_addr, &len);
    if (conn->bt_handler < 0)
        return -1;
    conn->ser_handler = openSerial(portName);
    if (conn->ser_handler < 0)
        return -1;
#ifdef _DEBUG_
    printf("Connect!\n");
#endif

    FD_ZERO(&inSet);
    maxfd = (conn->bt_handler > conn->ser_handler)? conn->bt_handler: 
        conn->ser_handler;
    outBytes = 0;
    do {
        FD_SET(conn->bt_handler, &inSet);
        FD_SET(conn->ser_handler, &inSet);
        tv.tv_sec = 0;
        tv.tv_usec = waitDelay;
        result = select(maxfd + 1, &inSet, NULL, NULL, &tv);
        if (result > 0) {
            if (FD_ISSET(conn->bt_handler, &inSet)) {
                if ((nbytes = read(conn->bt_handler, inBuf, sizeof(inBuf))) > 0)
                    result = write(conn->ser_handler, inBuf, nbytes);
#ifdef _DEBUG_
                printf("bt->ser %d\n", nbytes);
#endif
            }
            if (FD_ISSET(conn->ser_handler, &inSet)) {
                if ((nbytes = read(conn->ser_handler, 
                    outBuf + outBytes, sizeof(outBuf) - outBytes)) > 0) {
                    outBytes += nbytes;
                    if (outBytes > wrThresh)
                        result = btWrite(conn->bt_handler, outBuf, &outBytes);
                }
            }
        } else if (result == 0) {
            if (outBytes > 0) 
                result = btWrite(conn->bt_handler, outBuf, &outBytes);
        }
    } while (result == 0 || (result > 0 && nbytes > 0));
    if (nbytes <= 0)
        result = -1;
    close(conn->bt_handler);
    close(conn->ser_handler);
#ifdef _DEBUG_
    printf("Disconnect!\n");
#endif
    return 0;
}

//eof
