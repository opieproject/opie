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

#include <iostream>
using namespace std;

#ifdef QWS
#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>
#else
#include <qapplication.h>
#endif

#include <qobjectdefs.h>

#include "mainwin.h"
#include "server.h"

#include "global.h"


int main(int argc, char *argv[])
{
  cout << "Root dir = " << ROOT << endl;
#ifdef QWS
  QPEApplication a( argc, argv );
#else
  QApplication a( argc, argv );
#endif

#ifdef QWS
  // Disable suspend mode
  cout << "Disabling suspend mode" << endl;
  QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::DisableSuspend;
#endif

  MainWindow *win = new MainWindow();
  a.setMainWidget(win);
  win->show();

  a.exec();

#ifdef QWS
  // Reenable suspend mode
  cout << "Enabling suspend mode" << endl;
  QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
#endif
 #ifdef _DEBUG
  DumpUnfreed();
 #endif
}
