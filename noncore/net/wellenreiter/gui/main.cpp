/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "mainwindow.h"
#ifdef QWS
#include <opie2/oapplication.h>
#else
#include <qapplication.h>
#endif

#include <qmessagebox.h>
#include <qstringlist.h>

// ==> OProcess
#include <qdir.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qtextstream.h>

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

int main( int argc, char **argv )
{
    #ifdef QWS
    OApplication a( argc, argv, "Wellenreiter II" );
    #else
    QApplication a( argc, argv );
    #endif
    WellenreiterMainWindow* w = new WellenreiterMainWindow();
    #ifdef QWS
    a.showMainWidget( w );
    #else
    a.setMainWidget( w );
    w->setCaption( "Wellenreiter II" );
    w->show();
    #endif

    a.processEvents(); // show the window before doing the safety checks
    int result = -1;
    static int killed = false;

    bool check = true;
    for ( int i = 1; i < argc; ++i )
    {
        if ( !strcmp( "-nocheck", argv[i] ) )
        {
            qDebug( "-nocheck found" );
            check = false;
            break;
        }
    }

    if ( check )
    {
        // root check
        if ( getuid() )
        {
            qWarning( QObject::tr( "Wellenreiter: trying to run as non-root!" ) );
            result = QMessageBox::warning( w, " - Wellenreiter II -  (non-root)", QObject::tr( "You have started Wellenreiter II\n"
            "as non-root. You will have\nonly limited functionality.\nProceed anyway?" ),
            QMessageBox::Yes, QMessageBox::No );
            if ( result == QMessageBox::No ) return -1;
        }

        // dhcp check - NOT HERE! This really belongs as a static member to OProcess
        // and I want to call it like that: if ( OProcess::isRunning( QString& ) ) ...

        QString line;
        QDir d = QDir( "/proc" );
        QStringList dirs = d.entryList( QDir::Dirs );
        QStringList::Iterator it;
        for ( it = dirs.begin(); it != dirs.end(); ++it )
        {
            //qDebug( "next entry: %s", (const char*) *it );
            QFile file( "/proc/"+*it+"/cmdline" );
            file.open( IO_ReadOnly );
            if ( !file.isOpen() ) continue;
            QTextStream t( &file );
            line = t.readLine();
            //qDebug( "cmdline = %s", (const char*) line );
            if ( line.contains( "dhcp" ) ) break;
        }
        if ( line.contains( "dhcp" ) )
        {
            qWarning( "Wellenreiter: found dhcp process #%d", (*it).toInt() );
            result = QMessageBox::warning( w, " - Wellenreiter II -  (dhcp)", QObject::tr( "You have a dhcp client running.\n"
            "This can severly limit scanning!\nShould I kill it for you?" ),
            QMessageBox::Yes, QMessageBox::No );
            if ( result == QMessageBox::Yes )
            {
                if ( -1 == ::kill( (*it).toInt(), SIGTERM ) )
                    qWarning( "Wellenreiter: can't kill process #%d (%s)", result, strerror( errno ) );
                else
                    killed = true;
            }
        }
    }

    a.exec();

    if ( check )
    {

        if ( killed )
        {
            result = QMessageBox::warning( w, " - Wellenreiter II -  (dhcp)", QObject::tr( "Restart your dhcp client?" ), QMessageBox::Yes, QMessageBox::No );
            if ( result == QMessageBox::Yes )
            {
                ::system( QString().sprintf( "dhclient &; udhcpcd &; dhcpcd &" ) );
            }
        }

        delete w;
    }
    return 0;
}
