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

#include "mainwindow.h"


#include <qdatetime.h>

#include <qpe/qpeapplication.h>

void myMessages( QtMsgType, const char*  ) {

}

int main( int argc, char **argv )
{
//    qInstallMsgHandler( myMessages );
    QPEApplication a( argc, argv );

    QTime time;
    time.start();
    Todo::MainWindow mw;
    int t = time.elapsed();
    qWarning("QTime %d", t/1000 );
    mw.setCaption("Opie Todolist");
    QObject::connect( &a, SIGNAL( flush() ), &mw, SLOT( slotFlush() ) );
    QObject::connect( &a, SIGNAL( reload() ), &mw, SLOT( slotReload() ) );

    a.showMainWidget(&mw);

    return a.exec();
}
