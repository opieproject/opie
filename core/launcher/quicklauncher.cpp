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

#include <qapplication.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <applnk.h>
#include <resource.h>
#include "quicklauncher.h"


void QuickLauncher::addLauncherButton( AppLnk *appLnk )
{
    QPushButton *pb = new QPushButton( this );
    // It's no a QObject anymore
    //pb->connect( pb, SIGNAL( clicked( ) ), appLnk, SLOT( execute() ) );
    pb->setPixmap( appLnk->pixmap() );
    pb->setFocusPolicy( QWidget::NoFocus );
    pb->setFlat( TRUE );
    pb->setFixedSize( 18, 18 );
}


QuickLauncher::QuickLauncher( QWidget *parent ) : QHBox( parent )
{
//  Example usage to add icons to the quick launcher area
//    addLauncherButton( "filebrowser_icon", "filebrowser" );
//    addLauncherButton( "textedit_icon", "textedit" );
//    addLauncherButton( "help_icon", "helpbrowser" );
}


