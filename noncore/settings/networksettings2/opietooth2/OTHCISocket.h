//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kdebluetooth@schaettgen.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef OTHCISOCKET_H
#define OTHCISOCKET_H

#include <qobject.h>
#include <qsocketnotifier.h>
#include <qsocketdevice.h>
#include <qguardedptr.h>

class QSocket;

namespace Opietooth2 {

/** Bluetooth HCI Socket class.
 * This class provides socket level access to the Bluez HCI layer.
 * It is set up to filter out all but HCI event packets, since more
 * is only allowed for privileged users.
 * @todo writeHciEvent() function
 * @todo error handling
 */

class OTDriver;

class OTHCISocket : public QObject {

    Q_OBJECT

public:

    OTHCISocket( OTDriver * ConnectTo );
    virtual ~OTHCISocket();

    /** Opens a HCI socket for the given
    @return true if sucessfully opened, false otherwise
    */
    virtual bool open();

    /** Closes the HCI socket. */
    virtual void close();

    bool sendCommand( unsigned char ogf, 
                      unsigned short ocf,
                      QByteArray buf
                    );
    bool readStatus( unsigned char ogf, 
                     unsigned short ocf, 
                     int *status, 
                     int timeout_ms = 1000);
    
    /** Reads whole HCI packet.
    @param packetType [out] The packet type. Should always be ...
    @param eventCode [out] The event code.
    @param buflen [in/out] The maximum size of the buffer / the number of
    bytes written into the buffer.
    @param paramBuf pointer to a buffer for the HCI event packet
    @return true if successful
    */
    /*bool readEvent(unsigned char &packetType,
                      unsigned char &eventCode, unsigned char &buflen,
                      char* paramBuf);*/

    enum Error { ErrSocket = 1 };

    /** Forces reading the next event packet. */
    void readEvent( void );

    /** Returns the internal socket */
    int socket( void );

    inline QSocketDevice & socketDevice() 
      { return HCISocket; }

    inline OTDriver * driver() const
      { return Driver; }

signals:

    void event( unsigned char eventCode, QByteArray buf);
    void error( QString message );
    void connectionClosed( );

private:

    void updateStatus( const QByteArray& data );

    //QSocketDevice hciSocket;
    QGuardedPtr<QSocketNotifier> HCIReadNotifier;
    QSocketDevice                HCISocket;
    OTDriver *                   Driver ;
    
    bool                         BStatusSet;
    unsigned short               LastStatusOcf;
    unsigned char                LastStatusOgf;
    int                          LastStatus;

private slots:

    void slotSocketActivated();
    void slotSocketError(int);
    void slotConnectionClosed();

};

};

#endif
