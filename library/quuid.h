/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

struct Q_EXPORT QUuid
{
    enum Variant {
        VarUnknown  =-1,
        NCS     = 0, // 0 - -
        DCE     = 2, // 1 0 -
        Microsoft   = 6, // 1 1 0
        Reserved    = 7  // 1 1 1
    };

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
        uint i;

        data1 = uuid.data1;
        data2 = uuid.data2;
        data3 = uuid.data3;
        for( i = 0; i < 8; i++ )
            data4[ i ] = uuid.data4[ i ];
    }

#ifndef QT_NO_QUUID_STRING
    QUuid( const QString & );
    QString toString() const;
#endif
    bool isNull() const;

    QUuid &operator=(const QUuid &orig )
    {
        uint i;

        data1 = orig.data1;
        data2 = orig.data2;
        data3 = orig.data3;
        for( i = 0; i < 8; i++ )
            data4[ i ] = orig.data4[ i ];

        return *this;
    }

    bool operator==(const QUuid &orig ) const
    {
        uint i;
        if ( data1 != orig.data1 || data2 != orig.data2 ||
                data3 != orig.data3 )
            return FALSE;

        for( i = 0; i < 8; i++ )
            if ( data4[i] != orig.data4[i] )
                return FALSE;

        return TRUE;
    }

    bool operator!=(const QUuid &orig ) const
    {
        return !( *this == orig );
    }

    bool operator<(const QUuid &other ) const;
    bool operator>(const QUuid &other ) const;

    QUuid::Variant variant() const;

    ulong   data1;
    ushort  data2;
    ushort  data3;
    uchar   data4[ 8 ];
};

#endif //QUUID_H
