/****************************************************************************
** $Id: qucomextra_p.h,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Definition of extra QUcom classes
**
** Created : 990101
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QUCOMEXTRA_H
#define QUCOMEXTRA_H

#ifndef QT_H
#include <private/qucom_p.h>
#endif // QT_H


class QVariant;

#ifndef  QT_NO_VARIANT
// 6dc75d58-a1d9-4417-b591-d45c63a3a4ea
extern const QUuid TID_QUType_QVariant;

struct Q_EXPORT QUType_QVariant : public QUType
{
    const QUuid *uuid() const;
    const char *desc() const;

    void set( QUObject *, const QVariant & );
    QVariant &get( QUObject * o );

    bool canConvertFrom( QUObject *, QUType * );
    bool canConvertTo( QUObject *, QUType * );
    bool convertFrom( QUObject *, QUType * );
    bool convertTo( QUObject *, QUType * );
    void clear( QUObject * );
    int serializeTo( QUObject *, QUBuffer * );
    int serializeFrom( QUObject *, QUBuffer * );
};
extern Q_EXPORT QUType_QVariant static_QUType_QVariant;
#endif //QT_NO_VARIANT


// {0x8d48b3a8, 0xbd7f, 0x11d5, 0x8d, 0x74, 0x00, 0xc0, 0xf0, 0x3b, 0xc0, 0xf3 }
extern Q_EXPORT const QUuid TID_QUType_varptr;
struct Q_EXPORT QUType_varptr : public QUType
{
    const QUuid *uuid() const;
    const char *desc() const;

    void set( QUObject *, const void* );
    void* &get( QUObject * o ) { return o->payload.ptr; }
    bool canConvertFrom( QUObject *, QUType * );
    bool canConvertTo( QUObject *, QUType * );
    bool convertFrom( QUObject *, QUType * );
    bool convertTo( QUObject *, QUType * );
    void clear( QUObject * ) {}
    int serializeTo( QUObject *, QUBuffer * );
    int serializeFrom( QUObject *, QUBuffer * );
};
extern Q_EXPORT QUType_varptr static_QUType_varptr;


#endif // QUCOMEXTRA_H

