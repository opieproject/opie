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

#ifndef OTATTRIBUTE_H
#define OTATTRIBUTE_H

#include <stdio.h>
#include <qstring.h>
#include <qarray.h>
#include <qvector.h>
#include <bluezlib.h>
#include <OTUUID.h>

namespace Opietooth2 {

class OTSDPAttribute;
class OTUUID;

typedef QVector<OTSDPAttribute> AttributeVector;

/**
@author Fred Schaettgen
*/
class OTSDPAttribute {

public:

    enum AttrType {
        INVALID     = 0,  
        NIL         = 1, 
        UINT        = 2, 
        INT         = 3, 
        UUID        = 4, 
        BOOLEAN     = 5, 
        STRING      = 6,
        SEQUENCE    = 7, 
        ALTERNATIVE = 8, 
        URL         = 9, 
        UNKNOWN     = 10
    };

    class int128_t {
    public :
        int128_t(int64_t l=0, int64_t h=0) {
            hi = h;
            lo = l;
        }
        int128_t(const OTSDPAttribute::int128_t & l) {
            hi = l.hi;
            lo = l.lo;
        }
        QString toString() const {
          char Buf[50];
          sprintf( Buf, "%lld%lld", hi, lo );
          return QString( Buf );
        }
        int64_t hi;
        int64_t lo;
    };

    class uint128_t {
    public :
        uint128_t( uint64_t l=0, uint64_t h=0) {
            hi = h;
            lo = l;
        }
        uint128_t( const OTSDPAttribute::uint128_t & l) {
            hi = l.hi;
            lo = l.lo;
        }
        QString toString() const {
          char Buf[50];
          sprintf( Buf, "%llu%llu", hi, lo );
          return QString( Buf );
        }
        uint64_t hi;
        uint64_t lo;
    };

public:

    OTSDPAttribute();
    OTSDPAttribute( sdp_data_t * D );
    ~OTSDPAttribute();

    QString toString( void );

    void setNil();
    void setInt(const OTSDPAttribute::int128_t & val);
    void setUInt(const OTSDPAttribute::uint128_t & val);
    void setUUID( const OTUUID & val);
    void setBool(bool val);
    void setString(const QString & val);
    void setURL(const QString & val);
    void setSequence(const AttributeVector& val);
    void setAlternative(const AttributeVector& val);

    QString getString();
    QString getURL();
    const OTSDPAttribute::int128_t & getInt();
    const OTSDPAttribute::uint128_t & getUInt();
    const OTUUID & getUUID();
    bool getBool();
    AttributeVector * getSequence();
    AttributeVector * getAlternative();

    UUIDVector getAllUUIDs();

    inline AttrType getType()
      { return type; }

    //QString getValString();
    const char * getTypeString();

private:

    AttrType  type;

    union {
      OTSDPAttribute::int128_t * intVal;
      OTSDPAttribute::uint128_t * uintVal;
      OTUUID *  uuidVal;
      bool      boolVal;
      QString * stringVal;     // strings and urls
      AttributeVector * sequenceVal;   // sequences and alternatives
    } Value;

};

};

#endif
