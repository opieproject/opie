/*
                             This file is part of the OPIE Project

               =.            Copyright (c)  2002 Andy Qua <andy.qua@blueyonder.co.uk>
             .=l.                                Dan Williams <drw@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

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


/* be less intrusive for translation -zecke */
extern QString LOCAL_SERVER;
extern QString LOCAL_IPKGS;

int main(int argc, char *argv[])
{
#ifdef QWS
  QPEApplication a( argc, argv );
#else
  QApplication a( argc, argv );
#endif

#ifdef QWS
  // Disable suspend mode
  QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::DisableSuspend;
#endif

  LOCAL_SERVER = QObject::tr("Installed packages");
  LOCAL_IPKGS = QObject::tr("Local packages");

  MainWindow *win = new MainWindow();
  a.setMainWidget(win);
  win->showMaximized();

  a.exec();

#ifdef QWS
  // Reenable suspend mode
  QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
#endif
 #ifdef _DEBUG
  DumpUnfreed();
 #endif
}
