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
    w->show();
    #endif
    a.exec();
    delete w;
    return 0;
}
