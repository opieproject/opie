/***************************************************************************
                          helpwindow.cpp  -  description
                             -------------------
    begin                : Sun Sep 8 2002
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

#include <qwidget.h>
#include <qlayout.h>
#include <qstring.h>
#include <qtextview.h>

#include "helpwindow.h"


#define HELP_TEXT \
"<qt><h1>SFCave Help</h1><p> " \
"SFCave is a flying game for the Zaurus.<br><br> " \
"The aim is to stay alive for as long as possible and get the highest score " \
"you can.<br><br>" \
"There are three game types currently - SFCave, Gates, and Fly.<br>" \
"<b>SFCave</b> is a remake of the classic SFCave game - fly through the " \
"cavern avoiding all the blocks that just happen to be hanging in " \
"midair<br><br>" \
"<b>Gates</b> is similar to SFCave but instead you must fly through the " \
"without crashing.<br><br>" \
"<b>Fly</b> is somewhat different to SFCave and above. Instead, you have " \
"are flying in the open air above a scrolling landscape, and the aim is to " \
"hug the ground - the closer to the land you fly the more points " \
"scored.<br><br><br>" \
"Basic instruction - Press <b>Up</B> or <b>Down</b> on the circle pad to " \
"start a new game, press the middle of the pad to apply thrust (makes you " \
"go up), and release the pad to remove thrust and drop down.<br><br>" \
"Also, if playing the Fly game, you can press z to toggle the display " \
"of the scoring zones. This will display 4 red lines at varying heights " \
"above the landscape - if your ship falls into this zone, point are scored. " \
"The closer to the landscape you get the more points you get.<br><br>" \
"In addition, SFCave has replays - save and load too so you can show off to all " \
"your friends (or vice versa). Currently, this is in its infancy but will improve.<br>" \
"To use, once you have crashed, press 'r' to replay the last game.<br>" \
"To save the replay press 's'.<br>" \
"To load a saved replay press 'l' (after you've crashed at least once).<br><br>" \
"Replays are currently saved to your home directory in a file called sfcave.replay." \
"This file can be copied and given to others as long as it it put in their home directory.<br><br>" \
"Have lots of fun.<br>" \
"Andy" \
"</p></qt>"


HelpWindow::HelpWindow( QWidget *parent, const char *name, bool modal, WFlags flags )
    : QDialog( parent, name, modal, flags )
{
//    resize( 230, 280 );

    setCaption( "Help for AQPkg" );

    QVBoxLayout *layout = new QVBoxLayout( this );
    QString text = HELP_TEXT;;
    QTextView *view = new QTextView( text, 0, this, "view" );
    layout->insertSpacing( -1, 5 );
    layout->insertWidget( -1, view );
    layout->insertSpacing( -1, 5 );

    showMaximized();
}

HelpWindow::~HelpWindow()
{
}
