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

#ifndef __packageslave_h__
#define __packageslave_h__

#include <qobject.h>

class QCopChannel;

class PackageSlave : public QObject
{
    Q_OBJECT

public:
    PackageSlave( QObject *parent, char* name = 0 );

protected:
    void installPackage( const QString &package  );
    void removePackage( const QString &package );

protected slots:
    void qcopMessage( const QCString &msg, const QByteArray &data );

private:
    void sendReply( const QCString& msg, const QString& arg );

private:
    QCopChannel *packageChannel;
};


#endif // __QUICK_LAUNCHER_H__


