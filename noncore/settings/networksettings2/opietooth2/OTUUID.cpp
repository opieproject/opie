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

#include <assert.h>
#include <qregexp.h>
#include <opie2/odebug.h>

#include <OTUUID.h>

using namespace Opietooth2;

OTUUID::OTUUID(QString s) {
        fromString(s);
}

OTUUID::OTUUID(uint64_t l, uint64_t h) {
        this->hi = h;
        this->lo = l;
}

OTUUID::OTUUID( const OTUUID & O ) {
        this->hi = O.hi;
        this->lo = O.lo;
}

OTUUID & OTUUID::operator =( const OTUUID & O ) {
        hi = O.hi;
        lo = O.lo;
        return *this;
}

bool OTUUID::fromString(QString s) {

    // Strip leading "0x"
    if (s.startsWith("0x"))
    {
        s = s.right(s.length()-2);
    }
    // Remove separators
    s.replace( QRegExp(":"), "" );

    bool bOk = false;
    if( s.length() == 4 || s.length() == 8) {

        uint32_t u32 = s.toUInt(&bOk, 16);
        setUUID32(u32);
        return bOk;

    } else if (s.length() == 32) {

#if (QT_VERSION >= 0x030200)
        uint64_t u64hi = s.left(16).toULongLong(&bOk, 16);
#else
        uint64_t u64hi = s.left(16).toULong(&bOk, 16);
#endif
        if (!bOk)
            return false;

#if (QT_VERSION >= 0x030200)
        uint64_t u64lo = s.right(16).toULongLong(&bOk, 16);
#else
        uint64_t u64lo = s.right(16).toULong(&bOk, 16);
#endif
        if (!bOk)
            return false;
        hi = u64hi;
        lo = u64lo;
        return true;

    } else {
        hi = 0;
        lo = 0;
        return true;
    }
}

void OTUUID::setUUID32(uint32_t v) {

    hi = uint64_t(0x1000) | (uint64_t(v) << 32);
    // see BT 1.1 Core spec p2.7.1
    lo = (uint64_t(0x80000080) << 32) | uint64_t(0x5F9B34FB);

}

void OTUUID::setUUID128(uint64_t _hi, uint64_t _lo) {
    hi = _hi;
    lo = _lo;
}

QString OTUUID::toString() const {
    QString ret;
    uint32_t v1 = uint32_t(hi >> 32);
    uint32_t v2 = uint32_t(hi & 0xFFFFFFFF);
    uint32_t v3 = uint32_t(lo >> 32);
    uint32_t v4 = uint32_t(lo & 0xFFFFFFFF);
    ret.sprintf("0x%08lx:%08lx:%08lx:%08lx",
                     (unsigned long)v1, (unsigned long)v2,
                     (unsigned long)v3, (unsigned long)v4);

    return ret;
}

OTUUID::operator QString() const {
    return toString();
}

OTUUID::operator ::uuid_t() const {

    //kdDebug() << "uuid_t() -> " << QString(*this) << endl;
    ::uuid_t ret;
    if ((lo == (uint64_t(0x80000080) << 32) | uint64_t(0x5F9B34FB)) &&
        ((hi&0xFFFFFFFF) == 0x1000)) {
        uint32_t uuid32val = uint32_t(hi >> 32);    
        if (uuid32val > 0xFFFF) {
            ret.type = SDP_UUID16;
            ret.value.uuid16 = uint16_t(uuid32val);
        } else {
            ret.type = SDP_UUID32;
            ret.value.uuid32 = uuid32val;
        }
    } else {
        ret.value.uuid128.data[0] = (lo >> 0) && 0xFF;
        ret.value.uuid128.data[1] = (lo >> 8) && 0xFF;
        ret.value.uuid128.data[2] = (lo >> 16) && 0xFF;
        ret.value.uuid128.data[3] = (lo >> 24) && 0xFF;
        ret.value.uuid128.data[4] = (lo >> 32) && 0xFF;
        ret.value.uuid128.data[5] = (lo >> 40) && 0xFF;
        ret.value.uuid128.data[6] = (lo >> 48) && 0xFF;
        ret.value.uuid128.data[7] = (lo >> 56) && 0xFF;
    
        ret.value.uuid128.data[8] = (hi >> 0) && 0xFF;
        ret.value.uuid128.data[9] = (hi >> 8) && 0xFF;
        ret.value.uuid128.data[10] = (hi >> 16) && 0xFF;
        ret.value.uuid128.data[11] = (hi >> 24) && 0xFF;
        ret.value.uuid128.data[12] = (hi >> 32) && 0xFF;
        ret.value.uuid128.data[13] = (hi >> 40) && 0xFF;
        ret.value.uuid128.data[14] = (hi >> 48) && 0xFF;
        ret.value.uuid128.data[15] = (hi >> 56) && 0xFF;
    
        ret.type = SDP_UUID128;
    }
    return ret;
}

bool OTUUID::operator<(const OTUUID & other) const {
    if (hi != other.hi) 
       return hi < other.hi;

    return lo<other.lo;
}

bool OTUUID::operator ==(const OTUUID& u) const {
    return (u.hi == hi) && (u.lo == lo);
}
