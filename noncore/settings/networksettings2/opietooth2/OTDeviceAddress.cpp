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

#include <string.h>
#include <bluezlib.h>
#include <opie2/odebug.h>

#include <OTDeviceAddress.h>

using namespace Opietooth2;

//const bdaddr_t OTDeviceAddress::bdaddr_any = {{0,0,0,0,0,0}};
//const bdaddr_t OTDeviceAddress::bdaddr_local = {{0, 0, 0, 0xff, 0xff, 0xff}};

const OTDeviceAddress OTDeviceAddress::invalid = 
                OTDeviceAddress();
const OTDeviceAddress OTDeviceAddress::any = 
                OTDeviceAddress("00:00:00:00:00:00");
const OTDeviceAddress OTDeviceAddress::local =  
                OTDeviceAddress("00:00:00:FF:FF:FF");

OTDeviceAddress::OTDeviceAddress() {

    IsValid = false;
    memset( &BDaddr, 0, sizeof( BDaddr ) );
}

OTDeviceAddress::OTDeviceAddress( const bdaddr_t& bdaddr, 
                                  bool networkbyteorder) {
    setBDAddr( bdaddr, networkbyteorder );
}

OTDeviceAddress::OTDeviceAddress(const QString& s) {

    bdaddr_t a;
    int ret = str2ba(s.latin1(), &a);
    if (ret==0) {
        IsValid = true;
        bacpy( &BDaddr, &a );
    } else {
        IsValid = false;
        bacpy( &BDaddr, &(OTDeviceAddress::invalid.getBDAddr()) ) ;
    }
}

QString OTDeviceAddress::toString() const {
    char addrbuf[20];
    ba2str(&BDaddr, addrbuf);
    return QString(addrbuf);

}

void OTDeviceAddress::setBDAddr( const bdaddr_t& bdaddr, 
                                 bool networkbyteorder) {
    if (networkbyteorder) {
        baswap(&BDaddr, &bdaddr);
    } else {
        bacpy( &BDaddr, &bdaddr );
    }
    IsValid = true;
}

bdaddr_t OTDeviceAddress::getBDAddrInNetworkByteOrder(void) const {
    bdaddr_t ret;
    baswap(&ret, &BDaddr);
    return ret;
}

bool OTDeviceAddress::operator==(const OTDeviceAddress& b) const {
    if ( ! IsValid && ! b.IsValid )
        return true;
    return memcmp( &BDaddr, &(b.BDaddr), sizeof( b.BDaddr ) ) == 0;
}

bool OTDeviceAddress::operator<(const OTDeviceAddress& b) const {
    if ( ! IsValid &&  ! b.IsValid )
        return false;
    else if ( ! IsValid && b.IsValid )
        return false;
    else if ( IsValid && ! b.IsValid )
        return true;

    if (BDaddr.b[5] != b.BDaddr.b[5]) 
        return (BDaddr.b[5] < b.BDaddr.b[5]);

    if (BDaddr.b[4] != b.BDaddr.b[4]) 
        return (BDaddr.b[4] < b.BDaddr.b[4]);

    if (BDaddr.b[3] != b.BDaddr.b[3]) 
        return (BDaddr.b[3] < b.BDaddr.b[3]);

    if (BDaddr.b[2] != b.BDaddr.b[2]) 
        return (BDaddr.b[2] < b.BDaddr.b[2]);

    if (BDaddr.b[1] != b.BDaddr.b[1]) 
        return (BDaddr.b[1] < b.BDaddr.b[1]);

    if (BDaddr.b[0] != b.BDaddr.b[0]) 
        return (BDaddr.b[0] < b.BDaddr.b[0]);

    return false;
}
