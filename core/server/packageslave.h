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

#ifndef __packageslave_h__
#define __packageslave_h__

#include <qtopia/global.h>
#include <qobject.h>

class QCopChannel;
class QProcess;

class PackageHandler : public QObject
{
    Q_OBJECT

public:
    PackageHandler( QObject *parent, char* name = 0 );

public slots:
    void redoPackages();

protected:
    void installPackage( const QString &package );
    void removePackage( const QString &package );

    void addPackageFiles( const QString &location, const QString &listfile );
    void addPackages( const QString &location );

    void cleanupPackageFiles( const QString &listfile );
    void cleanupPackages( const QString &location );

    void prepareInstall( const QString& size, const QString& path );

protected slots:
    void qcopMessage( const QCString &msg, const QByteArray &data );
    void iProcessExited();
    void rmProcessExited();
    void readyReadStdout();
    void readyReadStderr();

private:
    void sendReply( const QCString& msg, const QString& arg );

private:
    QCopChannel *packageChannel;
    QProcess *currentProcess;
    QString currentPackage;
    QString currentProcessError;
    bool mNoSpaceLeft;
};


#endif // __QUICK_LAUNCHER_H__


