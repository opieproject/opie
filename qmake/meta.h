/****************************************************************************
** 
**
** Definition of QMakeMetaInfo class.
**
** Copyright (C) 1992-2003 Trolltech AS.  All rights reserved.
**
** This file is part of qmake.
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
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
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

#ifndef __META_H__
#define __META_H__

#include <qmap.h>
#include <qstringlist.h>
#include <qstring.h>

class QMakeMetaInfo 
{
    bool readLibtoolFile(const QString &f);
    bool readPkgCfgFile(const QString &f);
    QMap<QString, QStringList> vars;
    QString meta_type;
    static QMap<QString, QMap<QString, QStringList> > cache_vars;
    void clear();
public:
    QMakeMetaInfo();

    bool readLib(const QString &lib);
    static QString findLib(const QString &lib);
    static bool libExists(const QString &lib);
    QString type() const;

    bool isEmpty(const QString &v);
    QStringList &values(const QString &v);
    QString first(const QString &v);
    QMap<QString, QStringList> &variables();
};

inline bool QMakeMetaInfo::isEmpty(const QString &v)
{ return !vars.contains(v) || vars[v].isEmpty(); }

inline QString QMakeMetaInfo::type() const
{ return meta_type; }

inline QStringList &QMakeMetaInfo::values(const QString &v)
{ return vars[v]; }

inline QString QMakeMetaInfo::first(const QString &v)
{
#if defined(Q_CC_SUN) && (__SUNPRO_CC == 0x500) || defined(Q_CC_HP)
    // workaround for Sun WorkShop 5.0 bug fixed in Forte 6
    if (isEmpty(v))
	return QString("");
    else
	return vars[v].first();
#else
    return isEmpty(v) ? QString("") : vars[v].first();
#endif
}

inline QMap<QString, QStringList> &QMakeMetaInfo::variables()
{ return vars; }

inline bool QMakeMetaInfo::libExists(const QString &lib)
{ return !findLib(lib).isNull(); }

#endif /* __META_H__ */
