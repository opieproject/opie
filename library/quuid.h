/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QUUID_H
#define QUUID_H

#ifndef QT_H
#include <qstring.h>
#endif // QT_H

#include <memory.h>

#if defined(Q_OS_WIN32)
#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID
{
    ulong   Data1;
    ushort  Data2;
    ushort  Data3;
    uchar   Data4[ 8 ];
} GUID;
#endif
#endif

#if defined( Q_WS_QWS ) && !defined( UUID_H_INCLUDED )
typedef unsigned char uuid_t[16];
#endif

struct Q_EXPORT QUuid
{
    QUuid()
    {
	memset( this, 0, sizeof(QUuid) );
    }
    QUuid( uint l, ushort w1, ushort w2, uchar b1, uchar b2, uchar b3, uchar b4, uchar b5, uchar b6, uchar b7, uchar b8 )
    {
	data1 = l;
	data2 = w1;
	data3 = w2;
	data4[0] = b1;
	data4[1] = b2;
	data4[2] = b3;
	data4[3] = b4;
	data4[4] = b5;
	data4[5] = b6;
	data4[6] = b7;
	data4[7] = b8;
    }
    QUuid( const QUuid &uuid )
    {
	memcpy( this, &uuid, sizeof(QUuid) );
    }
#ifndef QT_NO_QUUID_STRING
    QUuid( const QString & );
    QString toString() const;
#endif
    bool isNull() const;

    QUuid &operator=(const QUuid &orig )
    {
	memcpy( this, &orig, sizeof(QUuid) );
	return *this;
    }

    bool operator==(const QUuid &orig ) const
    {
	return !memcmp( this, &orig, sizeof(QUuid) );
    }

    bool operator!=(const QUuid &orig ) const
    {
	return !( *this == orig );
    }

#if defined(Q_OS_WIN32)
    // On Windows we have a type GUID that is used by the platform API, so we
    // provide convenience operators to cast from and to this type.
    QUuid( const GUID &guid )
    {
	memcpy( this, &guid, sizeof(GUID) );
    }

    QUuid &operator=(const GUID &orig )
    {
	memcpy( this, &orig, sizeof(QUuid) );
	return *this;
    }

    operator GUID() const
    {
	GUID guid = { data1, data2, data3, { data4[0], data4[1], data4[2], data4[3], data4[4], data4[5], data4[6], data4[7] } };
	return guid;
    }

    bool operator==( const GUID &guid ) const
    {
	return !memcmp( this, &guid, sizeof(QUuid) );
    }

    bool operator!=( const GUID &guid ) const
    {
	return !( *this == guid );
    }
#endif
#if defined (Q_WS_QWS)
    QUuid( uuid_t uuid ) 
    {
	memcpy( this, &uuid, sizeof(uuid_t) );
    }

    QUuid &operator=(const uuid_t &orig )
    {
	memcpy( this, &orig, sizeof(uuid_t) );
	return *this;
    }
#endif
    
    ulong   data1;
    ushort  data2;
    ushort  data3;
    uchar   data4[ 8 ];
};

#endif //QUUID_H
