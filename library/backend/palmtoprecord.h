/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included
** in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
** THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A
** PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QTPALMTOP_RECORD_H
#define QTPALMTOP_RECORD_H

#include <qglobal.h>
#include "qpcglobal.h"
#include "palmtopuidgen.h"
#include <qarray.h>
#include <qmap.h>

#if defined(QPC_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QPC_EXPORT QMap<QString, QString>;
// MOC_SKIP_END
#endif

class QRegExp;
namespace Qtopia {

class RecordPrivate;
class QPC_EXPORT Record
{
public:
    Record() : mUid(0), mCats() { }
    Record( const Record &c ) :	mUid( c.mUid ), mCats ( c.mCats ), customMap(c.customMap) { }
    virtual ~Record() { }

    Record &operator=( const Record &c );

    virtual bool match( const QRegExp & ) const { return FALSE; }

    void setCategories( const QArray<int> &v ) { mCats = v; }
    void setCategories( int single );
    const QArray<int> &categories() const { return mCats; }

    int uid() const { return mUid; };
    virtual void setUid( int i ) { mUid = i; uidGen().store( mUid ); }
    bool isValidUid() const { return mUid != 0; }
    void assignUid() { setUid( uidGen().generate() ); }

    virtual QString customField(const QString &) const;
    virtual void setCustomField(const QString &, const QString &);
    virtual void removeCustomField(const QString &);

    virtual bool operator == ( const Record &r ) const
{ return mUid == r.mUid; }
    virtual bool operator != ( const Record &r ) const
{ return mUid != r.mUid; }

    // convenience methods provided for loading and saving to xml
    static QString idsToString( const QArray<int> &ids );
    // convenience methods provided for loading and saving to xml
    static QArray<int> idsFromString( const QString &str );

    // for debugging
    static void dump( const QMap<int, QString> &map );

protected:
    virtual UidGen &uidGen() = 0;

    virtual QString customToXml() const;

private:
    int mUid;
    QArray<int> mCats;

    QMap<QString, QString> customMap;

    RecordPrivate *d;
};

}

#endif
