/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** Licensees holding valid Qtopia Developer license may use this
** file in accordance with the Qtopia Developer License Agreement
** provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
** THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE.
**
** email sales@trolltech.com for information about Qtopia License
** Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <qpe/qpeapplication.h>
#include <qpe/fileselector.h>
#include "mainwindow.h"

int main( int argc, char ** argv )
{
    QPEApplication a( argc, argv );

    MainWindow e;
    a.showMainDocumentWidget(&e);

    QObject::connect( &a, SIGNAL( lastWindowClosed() ),
		      &a, SLOT( quit() ) );

    a.exec();
}
