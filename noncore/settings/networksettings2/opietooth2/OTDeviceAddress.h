//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kdebluetooth@0xF.de                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef OTDEVICEADDRESS_H
#define OTDEVICEADDRESS_H

#include <qstring.h>
#include <bluezlib.h>

namespace Opietooth2 {

/** Represents a Bluetooth device address.
 * This class allows easy conversion of Bluetooth addresses
 * from and to strings. It also works with the bdaddr_t type
 * used by BlueZ.
 * The class offers a comparsion and less-than operator, so
 * that it can be used a key in an STL map or similar.
 * The string conversion functions of BlueZ are used internally,
 * so the string format is the same than for BlueZ's ba2str().
 */
class OTDeviceAddress {

public:

    /** Default constructor.
    The resulting address is equal to DeviceAddress::invalid.
    */
    OTDeviceAddress();

    /** Initialize the object with a BlueZ bdaddr_t.
    @param bdaddr address
    @param networkbyteorder if true, bdaddr is assumed to be in
    network byte order and converted to host byte order first.
    */
    explicit OTDeviceAddress( const bdaddr_t& bdaddr,
                              bool networkbyteorder = false
                            );

    /** Initializes the object with an address given as a string.
    The string must be in a format which is understood by the
    BlueZ str2ba function, like 00:61:58:4C:E6:AD. Case doesn't matter.
    @param s address string
    */
    explicit OTDeviceAddress(const QString& s);

    /** convert the address to a string.
    @return address string
    */
    QString toString() const;

    /** Converts the DeviceAddress into a BlueZ bdaddr_t.
    @param networkbyteorder if true, the address is returned
        in network byte order.
    @return the address as bdaddr_t
    */
    inline const bdaddr_t & getBDAddr( void ) const
      { return BDaddr; }
    bdaddr_t getBDAddrInNetworkByteOrder() const;
    void setBDAddr( const bdaddr_t& bdaddr,
                    bool networkbyteorder = false
                  );

    /** Less-than-operator.
    Mainly there to use DeviceAddress inside STL containers,
    like STL sets or as a key in a STL map.
    @param b the DeviceAddress to compare to (right hand side)
    @return true if this < b, false otherwise.
    */
    bool operator<(const OTDeviceAddress& b) const;

    /** Equality operator.
    Tests two device addresses for equality.
    @param b the DeviceAddress to compare to (right hand side)
    @return true if this and b have the same address or
    if both are invalid, false otherwise
    */
    bool operator==(const OTDeviceAddress& b) const;

    inline bool operator!=(const OTDeviceAddress& b) const
      { return ! ( *this == b ); }

    /** The address 00:00:00:FF:FF:FF */
    static const OTDeviceAddress local;

    /** The address 00:00:00:00:00:00 */
    static const OTDeviceAddress any;

    /** An address tagged as invalid */
    static const OTDeviceAddress invalid;

protected:

    bdaddr_t BDaddr;
    bool     IsValid;

};

};

#endif
