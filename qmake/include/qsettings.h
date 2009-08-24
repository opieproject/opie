/****************************************************************************
**
**
** Definition of QSettings class
**
** Created : 000626
**
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
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

#ifndef QSETTINGS_H
#define QSETTINGS_H

#ifndef QT_H
#include "qdatetime.h"
#include "qstringlist.h"
#endif // QT_H

#ifndef QT_NO_SETTINGS

class QSettingsPrivate;


class Q_EXPORT QSettings
{
public:
    enum Format {
	Native = 0,
	Ini
    };
    enum System {
	Unix = 0,
	Windows,
	Mac
    };
    enum Scope {
	User,
	Global
    };

    QSettings();
    QSettings( Format format );

    ~QSettings();

#if !defined(Q_NO_BOOL_TYPE)
    bool	writeEntry( const QString &, bool );
#endif
    bool	writeEntry( const QString &, double );
    bool	writeEntry( const QString &, int );
    bool	writeEntry( const QString &, const char * );
    bool	writeEntry( const QString &, const QString & );
    bool	writeEntry( const QString &, const QStringList & );
    bool	writeEntry( const QString &, const QStringList &, const QChar& sep );

    QStringList entryList(const QString &) const;
    QStringList subkeyList(const QString &) const;

    //### remove non const versions in 4.0
    QStringList readListEntry( const QString &, bool * = 0 );
    QStringList readListEntry( const QString &, const QChar& sep, bool * = 0 );
    QString	readEntry( const QString &, const QString &def = QString::null, bool * = 0 );
    int		readNumEntry( const QString &, int def = 0, bool * = 0 );
    double	readDoubleEntry( const QString &, double def = 0, bool * = 0 );
    bool	readBoolEntry( const QString &, bool def = FALSE, bool * = 0 );

    //### make those non-inlined in 4.0
    QStringList readListEntry( const QString &key, bool *ok = 0 ) const
    {
	QSettings *that = (QSettings*)this;
	return that->readListEntry( key, ok );
    }
    QStringList readListEntry( const QString &key, const QChar& sep, bool *ok = 0 ) const
    {
	QSettings *that = (QSettings*)this;
	return that->readListEntry( key, sep, ok );
    }
    QString	readEntry( const QString &key, const QString &def = QString::null,
			   bool *ok = 0 ) const
    {
	QSettings *that = (QSettings*)this;
	return that->readEntry( key, def, ok );
    }
    int		readNumEntry( const QString &key, int def = 0, bool *ok = 0 ) const
    {
	QSettings *that = (QSettings*)this;
	return that->readNumEntry( key, def, ok );
    }

    double	readDoubleEntry( const QString &key, double def = 0, bool *ok = 0 ) const
    {
	QSettings *that = (QSettings*)this;
	return that->readDoubleEntry( key, def, ok );
    }
    bool	readBoolEntry( const QString &key, bool def = FALSE, bool *ok = 0 ) const
    {
	QSettings *that = (QSettings*)this;
	return that->readBoolEntry( key, def, ok );
    }

    bool	removeEntry( const QString & );

    void insertSearchPath( System, const QString & );
    void removeSearchPath( System, const QString & );

    void setPath( const QString &domain, const QString &product, Scope = Global );

    void beginGroup( const QString &group );
    void endGroup();
    void resetGroup();
    QString group() const;

    bool sync();

private:
    QSettingsPrivate *d;

#if defined(Q_DISABLE_COPY)
    QSettings(const QSettings &);
    QSettings &operator=(const QSettings &);
#endif

    QDateTime lastModificationTime( const QString & );

    friend class QApplication;
};

#endif // QT_NO_SETTINGS
#endif // QSETTINGS_H
