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

// To add a new app, include appropriate files and add another APP().
// No tr() anywhere in this file

#ifndef APP_INCLUDES
#define APP_INCLUDES
// pim apps
#include "../applications/addressbook/addressbook.h"
#include "../applications/datebook/datebook.h"
#include "../applications/todo/mainwindow.h"
#include "../applications/qtmail/qtmailwindow.h"

// general apps
#include "../libraries/qtopiacalc/calculator.h"
//#include "../applications/citytime/citytime.h" // where did this go?
#include "../applications/clock/clock.h"
#include "../applications/helpbrowser/helpbrowser.h"
#include "../applications/imageviewer/showimg.h"
#include "../applications/mediaplayer/maindocumentwidgetstack.h" // does not work
//#include "../applications/sysinfo/sysinfo.h" // does not work
#include "../applications/textedit/textedit.h"

// games
#include "../games/fifteen/fifteen.h"
#include "../games/go/gowidget.h"
#include "../games/mindbreaker/mindbreaker.h"
#include "../games/minesweep/minesweep.h"
#include "../games/qasteroids/toplevel.h"
#include "../games/snake/interface.h"
#include "../games/tetrix/qtetrix.h"
#include "../games/solitaire/canvascardwindow.h"
//#include "../games/parashoot/interface.h" // grabs keyboard, also -ldl??
//#include "../games/wordgame/wordgame.h" // does not work

// settings
#include "../settings/systemtime/settime.h"
#include "../settings/appearance/appearance.h"

/* #include "../settings/rotation/settings.h"
#include "../settings/language/settings.h"
#include "../settings/light-and-power/settings.h" */

/* uncommonly used apps
#include "../applications/filebrowser/filebrowser.h"
#include "../applications/embeddedkonsole/konsole.h" */

#endif

// APP(app-id   class   maximize?   documentary?)
// pim apps
APP( "addressbook",AddressbookWindow,1,0 )
APP( "datebook",DateBook,1,0 )
APP( "todolist",TodoWindow,1,0 )
APP( "qtmail",QTMailWindow,1,0 )

// general apps
APP( "calculator",Calculator,1,0 )
//APP( "citytime",CityTime,1,0 )
APP( "clock",Clock,1,0 )
APP( "helpbrowser",HelpBrowser,1,1 )
APP( "showimg",ImageViewer,1,0 )
APP( "mpegplayer",MainDocumentWidgetStack,1,0 )
//APP( "sysinfo",SystemInfo,1,0 )
APP( "textedit",TextEdit,1,1 )

// games
APP( "fifteen",FifteenMainWindow,1,0 )
APP( "go",GoMainWidget,1,0 )
APP( "mindbreaker",MindBreaker,1,0 )
APP( "minesweep",MineSweep,1,0 )
APP( "qasteroids",KAstTopLevel,1,0 )
APP( "snake",SnakeGame,1,0 )
APP( "tetrix",QTetrix,1,0 )
APP( "patience",CanvasCardWindow,1,0 )
//APP( "parashoot",ParaShoot,1,0 )
//APP( "wordgame",WordGame,1,0 )

// settings
APP( "systemtime",SetDateTime,1,0 )
APP( "appearance",AppearanceSettings,1,0 )
//APP( "light-and-power",LightSettings,1,0 )

/* APP( "sound",SoundSettings,1,0 ) */

/* uncommonly used apps
APP( "filebrowser",FileBrowser,1,0 )
APP( "embeddedkonsole",	Konsole,1,0 )
*/

