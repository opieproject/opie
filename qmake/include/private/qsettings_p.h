/****************************************************************************
** $Id: qsettings_p.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Definition of QSettings related classes
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the Qt GUI Toolkit.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses for Windows may use this file in accordance with the Qt Commercial
** License Agreement provided with the Software.
**
** This file is not available for use under any other license without
** express written permission from the copyright holder.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QSETTINGS_P_H
#define QSETTINGS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QSettings. This header file may change from version to 
// version without notice, or even be removed.
//
// We mean it.
//
//

#ifndef QT_H
#include "qstringlist.h"
#include "qmap.h"
#include "qvaluestack.h"
#endif // QT_H

class QSettingsSysPrivate;

// QSettingsGroup is a map of key/value pairs
class QSettingsGroup : public QMap<QString,QString>
{
public:
    QSettingsGroup();

    bool modified;
};

// QSettingsHeading is a map of heading/group pairs
class QSettingsHeading : public QMap<QString,QSettingsGroup>
{
public:
    QSettingsHeading::Iterator git;
    void read(const QString &);
    void parseLine(QTextStream &);
};


class QSettingsPrivate
{
public:
    QSettingsPrivate( QSettings::Format format );
    ~QSettingsPrivate();

    QSettingsGroup readGroup();
    void removeGroup(const QString &);
    void writeGroup(const QString &, const QString &);
    QDateTime modificationTime();

    QStringList searchPaths;
    QMap<QString,QSettingsHeading> headings;
    QString group;
    QString heading;

    /*### static data brings threading trouble
    static QString *defProduct;
    static QString *defDomain;
    */
    QValueStack<QString> groupStack;
    QString groupPrefix;

    bool groupDirty :1;
    bool modified :1;
    bool globalScope :1;

#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    // system dependent implementations to use the 
    // system specific setting database (ie. registry on Windows)

    QSettingsSysPrivate *sysd;
    void	sysInit();
    void	sysClear();

#if !defined(Q_NO_BOOL_TYPE)
    bool	sysWriteEntry( const QString &, bool );
#endif
    bool	sysWriteEntry( const QString &, double );
    bool	sysWriteEntry( const QString &, int );
    bool	sysWriteEntry( const QString &, const QString & );
    bool	sysWriteEntry( const QString &, const QStringList & );
    bool	sysWriteEntry( const QString &, const QStringList &, const QChar& sep );

    QStringList sysEntryList(const QString &) const;
    QStringList sysSubkeyList(const QString &) const;

    QStringList sysReadListEntry( const QString &, bool * = 0 ) const;
    QStringList sysReadListEntry( const QString &, const QChar& sep, bool * = 0 ) const;
    QString	sysReadEntry( const QString &, const QString &def = QString::null, bool * = 0 ) const;
    int		sysReadNumEntry( const QString &, int def = 0, bool * = 0 ) const;
    double	sysReadDoubleEntry( const QString &, double def = 0, bool * = 0 ) const;
    bool	sysReadBoolEntry( const QString &, bool def = 0, bool * = 0 ) const;

    bool	sysRemoveEntry( const QString & );

    bool	sysSync();

    void	sysInsertSearchPath( QSettings::System, const QString & );
    void	sysRemoveSearchPath( QSettings::System, const QString & );
#endif
};

#endif // QSETTINGS_P_H
