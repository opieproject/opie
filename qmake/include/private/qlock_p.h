/****************************************************************************
** $Id: qlock_p.h,v 1.1 2003-07-10 02:40:11 llornkcor Exp $
**
** Definition of QLock class. This manages interprocess locking
**
** Created : 20000406
**
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses for Qt/Embedded may use this file in accordance with the
** Qt Embedded Commercial License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QLOCK_P_H
#define QLOCK_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  This header file may
// change from version to version without notice, or even be
// removed.
//
// We mean it.
//
//

#ifndef QT_H
#include <qstring.h>
#endif // QT_H

class QLockData;

class QLock
{
public:
    QLock( const QString &filename, char id, bool create = FALSE );
    ~QLock();

    enum Type { Read, Write };

    bool isValid() const;
    void lock( Type type );
    void unlock();
    bool locked() const;

private:
    Type type;
    QLockData *data;
};


// Nice class for ensuring the lock is released.
// Just create one on the stack and the lock is automatically released
// when QLockHolder is destructed.
class QLockHolder
{
public:
    QLockHolder( QLock *l, QLock::Type type ) : qlock(l) {
	qlock->lock( type );
    }
    ~QLockHolder() { if ( locked() ) qlock->unlock(); }

    void lock( QLock::Type type ) { qlock->lock( type ); }
    void unlock() { qlock->unlock(); }
    bool locked() const { return qlock->locked(); }

private:
    QLock *qlock;
};

#endif

