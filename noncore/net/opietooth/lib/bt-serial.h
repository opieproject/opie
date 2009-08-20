/* $Id: bt-serial.h,v 1.1 2006-04-14 19:22:37 korovkin Exp $
 *	Bluetooth serial forwarder functions declaration
 *
 *	(c) Copyright 2006 GPL
 *
 *	This software is provided under the GNU public license, incorporated
 *	herein by reference. The software is provided without warranty or
 *	support.
 */
#ifndef _BT_SERIAL_H_
#define _BT_SERIAL_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef struct //bt-serial connection handler
{
    int bt_handler; //Bluetooth connection handler
    int ser_handler; //serial port handler
} BTSerialConn;

/*
 * bt_serialStart
 * Function starts bt-serial service
 * return 0 success -1 on error
 */
int bt_serialStart(void);

/*
 * bt_serialForward
 * Function forwards data received from bt-connection to serial and backward
 * conn - connection handler
 * portName - name of the port to connect
 * return 0 success -1 on error
 */
int bt_serialForward(BTSerialConn* conn, const char* portName);

/*
 * bt_serialStop
 * Function stops bt-serial service
 * return device handler on success -1 on error
 */
int bt_serialStop(void);

/*
 * Function opens and configures serial port
 * portName - name of the serial port
 * return 0 on success, -1 on error
 */
int openSerial(const char* portName);

#ifdef __cplusplus
}
#endif

#endif
//eof
