/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Mon Aug 26 13:32:30 BST 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef QWS
#include <qpe/qpeapplication.h>
#else
#include <qapplication.h>
#endif

#include <qobjectdefs.h>

#include "mainwin.h"
#include "server.h"

#include "global.h"


/*
int main2(int argc, char *argv[])
{
    Server local( "local", "", "status" );
    local.readPackageFile();

    Server s( "opiecvs", "aaa" );
    s.readPackageFile( &local );

}
*/

int main(int argc, char *argv[])
{
#ifdef QWS
  QPEApplication a( argc, argv );
#else
  QApplication a( argc, argv );
#endif

  MainWindow *win = new MainWindow();
  a.setMainWidget(win);
  win->show();

  a.exec();

 #ifdef _DEBUG
  DumpUnfreed();
 #endif
}
