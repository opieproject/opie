/****************************************************************************
**
** Definition of base class for all pointer based collection classes
**
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the Qt GUI Toolkit.
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

#ifndef QPTRCOLLECTION_H
#define QPTRCOLLECTION_H

#ifndef QT_H
#include "qglobal.h"
#endif // QT_H


class QGVector;
class QGList;
class QGDict;


class Q_EXPORT QPtrCollection			// inherited by all collections
{
public:
    bool autoDelete()	const	       { return del_item; }
    void setAutoDelete( bool enable )  { del_item = enable; }

    virtual uint  count() const = 0;
    virtual void  clear() = 0;			// delete all objects

    typedef void *Item;				// generic collection item

protected:
    QPtrCollection() { del_item = FALSE; }		// no deletion of objects
    QPtrCollection(const QPtrCollection &) { del_item = FALSE; }
    virtual ~QPtrCollection() {}

    bool del_item;				// default FALSE

    virtual Item     newItem( Item );		// create object
    virtual void     deleteItem( Item ) = 0;	// delete object
};


#ifndef QT_NO_COMPAT
#define QCollection QPtrCollection
#endif

#endif // QPTRCOLLECTION_H
