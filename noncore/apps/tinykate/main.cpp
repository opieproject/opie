/***************************************************************************
                             main.cpp
                             -------------------
    begin                : November 2002
    copyright            : (C) 2002 by Joseph Wenninger <jowenn@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation.                                         *
 *   ONLY VERSION 2 OF THE LICENSE IS APPLICABLE                           *
 *                                                                         *
 ***************************************************************************/
#include <qpe/qpeapplication.h>
#include <qmainwindow.h>
#include "tinykate.h"


int main( int argc, char ** argv )
{
    QPEApplication a( argc, argv );

    TinyKate *m = new TinyKate();
    a.setMainWidget( m );
    m->showMaximized();
    return a.exec();
}
