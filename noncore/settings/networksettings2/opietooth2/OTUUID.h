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

#ifndef OTUUID_H
#define OTUUID_H

#include <qstring.h>
#include <qarray.h>
#include <bluezlib.h>

namespace Opietooth2 {

class OTUUID;

typedef QArray<OTUUID> UUIDVector;

class OTUUID {

public :

    OTUUID( QString s );
    OTUUID( uint64_t l=0, uint64_t h=0);
    OTUUID( const OTUUID & O );

    bool fromString(QString s);

    void setUUID128(uint64_t hi, uint64_t lo);

    void setUUID32(uint32_t v);

    uint16_t toShort()
      { return ((hi>>32) & 0xffff); }
    uint32_t toLong()
      { return ((hi>>32) & 0xffffffff); }
    uint64_t toLongLong()
      { return hi; }

    QString toString() const ;
    operator QString() const;
    operator ::uuid_t() const;

    OTUUID & operator=( const OTUUID & other ) ;
    bool operator<( const OTUUID & other ) const;
    bool operator==(const OTUUID & uuid) const;

    uint64_t hi;
    uint64_t lo;
};
}
#endif
