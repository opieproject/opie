/***************************************************************************
 *   Copyright (C) 2003 by Mattia Merzi                                    *
 *   ottobit@ferrara.linux.it                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef OTDRIVER_H
#define OTDRIVER_H

/*
 * This class handles the networkSetup with the BlueZ libraries, implements
 * most of the bluetooth-related functions and owns the KBTDevice class
 * that is a single local bluetooth device representation.
 * It is used as a Singleton pattern thru the getDefaultConnector() method.
 *
 */
#include <qobject.h>
#include <opie2/odebug.h>
#include <bluezlib.h>

#include <OTDeviceAddress.h>

namespace Opietooth2 {

class OTGateway;
class OTHCISocket;
class OTDeviceAddress;
class OTPeer;

/*
 * This is the representation of a
 * Bluetooth device for the local
 * machine.
 */
class OTDriver : public QObject {

      Q_OBJECT

public:

      OTDriver( OTGateway * Ot, struct hci_dev_info* di);
      ~OTDriver();

      inline  OTGateway * gateway() const
        { return OT; }

      /*
       * Initializes the device using the passed hci_dev_info
       * struct, contining the basic information in order to
       * talk with the pyisical device.
       */
      void init(struct hci_dev_info* di);

      /*
       * Open the device and obtains a file descriptor to id
       * return if open
       */
      bool open();

      OTHCISocket * openSocket();

      void closeSocket();

      /*
       * Return true if the device is opened.
       */
      int isOpen() 
        { return Fd != -1; };

      /*
       * Calls the ioctl(HCIDEVRESET) in order to
       * reset the device.
       */
      int reset();

      // set Up if true and not yet up
      void setUp( bool Mode );

      /*
       * Turn the device up and the reinitializes the device.
       * If the device is already up, nothing is done.
       */
      void bringUp();

      /*
       * Turn the device down and the reinitializes the device.
       * If the device is already down, nothing is done.
       */
      void bringDown();

      // is the device UP now
      bool isUp() const 
         { return IsUp; }

      // get current state 
      // return 1 if UP, 0 if DOWN, -1 if ERROR
      int currentState();

      /*
       * Returns the name of the device. If the device is
       * down, the device id is returned.
       */
      QString name();

      int devId() const 
          { return Dev_id; };

      /*
       * Returns the string representation of the device id,
       * with the same format of the hciconfig tool.
       */
      QString devname() 
          { return Dev; };

      /*
       * Returns the file descriptor for the local
       * networkSetup to this device
       */
      int fd() const 
          { return Fd; };
      void setfd(int _fd) 
          { Fd = _fd; };

      void setType(int _type) 
         { Type = _type; };
      int type() const 
         { return Type; };
      QString strType();

      int iscan() const 
        { return Iscan; }
      void setIScan(int _iscan) 
        { Iscan = _iscan; }

      int pscan() const 
        { return Pscan; }
      void setPScan(int _pscan) 
        { Pscan = _pscan; }

      int authentication() const 
        { return Auth; }
      void setAuthentication(int _auth) 
        { Auth = _auth; }

      int encryption() const 
        { return Encrypt; }
      void setEncryption(int _encrypt) 
        { Encrypt = _encrypt; }

      void setDevId(int _dev_id) 
        { Dev_id = _dev_id; };
      void setDev(char* _dev) 
        { Dev = _dev; };

      void setFlags( unsigned long flags);

      const OTDeviceAddress & address() 
        { return Address; }

      void setFeatures( unsigned char * _f);
      QString features() 
        { return Features; }

      void setManufacturer(int compid);
      QString manufacturer() 
        { return Manufacturer; }

      QString revision();

      void setScanMode(bool iscan, bool pscan);
      void setClass(unsigned long cls);

      void changeDevName(const char* name);
      void changeEncryption(bool _encryption);
      void changeAuthentication(bool _auth);

      void getClass( QString & service,  
                     QString & device );
      void changeClass( unsigned char service, 
                        unsigned char major, 
                        unsigned char minor);

      QString getPeerName( const OTDeviceAddress & PAddr );

      // address must be connected to this driver
      long getLinkQuality( const OTDeviceAddress & Address );

signals :

      void error( const QString & );
      void stateChange( OTDriver * , bool );
      void driverDisappeared( OTDriver * );

private slots :

      /*
       * Reinitializes the device, obtaining a fresh
       * hci_dev_info structure.
       */
      void reinit();
      void SLOT_CloseFd();

private:

      QString getRevEricsson();
      QString getRevCsr(unsigned short rev);

      QString     Name;
      QString     Dev;
      QString     Revision;
      QString     Manufacturer;
      OTDeviceAddress  Address;
      QString     Features;
      QTimer  *   AutoClose;

      int         Dev_id,Fd,Type;
      bool        IsUp;
      int         Iscan,Pscan,Auth,Encrypt;

      // socket bound to this device
      OTHCISocket * Socket;

      // backpointer to opietooth system 
      OTGateway *   OT;
};

}
#endif
