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

#include "startmenu.h"
#include "sidething.h"
#include "mrulist.h"
#include "info.h"

#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <qpe/global.h>
#include <qpe/resource.h>

#include <qdict.h>

#include <stdlib.h>


// #define USE_CONFIG_FILE


StartMenu::StartMenu(QWidget *parent) : QLabel( parent )
{
    loadOptions();

    setPixmap( Resource::loadPixmap( startButtonPixmap ) );
    setFocusPolicy( NoFocus );
    //setFlat( startButtonIsFlat );

    apps = new AppLnkSet( QPEApplication::qpeDir() + "apps" );

    createMenu();
}


void StartMenu::mousePressEvent( QMouseEvent * )
{
    launch();
    if (desktopInfo)
        desktopInfo->menuClicked();
}


StartMenu::~StartMenu()
{
    delete apps;
}


void StartMenu::loadOptions()
{
#ifdef USE_CONFIG_FILE
    // Read configuration file
    Config config("StartMenu");
    config.setGroup( "StartMenu" );
    QString tmpBoolString1 = config.readEntry( "UseWidePopupMenu", "FALSE" );
    useWidePopupMenu  = ( tmpBoolString1 == "TRUE" ) ? TRUE : FALSE;
    QString tmpBoolString2 = config.readEntry( "StartButtonIsFlat", "TRUE" );
    startButtonIsFlat = ( tmpBoolString2 == "TRUE" ) ? TRUE : FALSE;
    QString tmpBoolString3 = config.readEntry( "UseMRUList", "TRUE" );
    popupMenuSidePixmap = config.readEntry( "PopupMenuSidePixmap", "sidebar" );
    startButtonPixmap = config.readEntry( "StartButtonPixmap", "go" );
#else
    // Basically just #include the .qpe_menu.conf file settings
    useWidePopupMenu = FALSE;
    popupMenuSidePixmap = "go";
    startButtonIsFlat = TRUE;
    startButtonPixmap = "start_button";
#endif
}


void StartMenu::createMenu()
{
    if ( useWidePopupMenu )
	launchMenu = new PopupWithLaunchSideThing( this, &popupMenuSidePixmap );
    else
        launchMenu = new StartPopupMenu( this );

    loadMenu( apps, launchMenu );

}

void StartMenu::itemSelected( int id )
{
    const AppLnk *app = apps->find( id );
    if ( app )
	app->execute();
}

bool StartMenu::loadMenu( AppLnkSet *folder, QPopupMenu *menu )
{
    bool result = FALSE;

    QStringList typs = folder->types();
    QDict<QPopupMenu> typpop;
    for (QStringList::Iterator tit=typs.begin(); tit!=typs.end(); ++tit) {
	if ( !(*tit).isEmpty() ) {
	    QPopupMenu *new_menu = new StartPopupMenu( menu );
	    typpop.insert(*tit, new_menu);
	    connect( new_menu, SIGNAL(activated(int)), SLOT(itemSelected(int)) );
	    menu->insertItem( folder->typePixmap(*tit), *tit, new_menu );
	}
    }

    QListIterator<AppLnk> it( folder->children() );
    for ( ; it.current(); ++it ) {
	AppLnk *app = it.current();
	if ( app->type() == "Separator" ) {
	    menu->insertSeparator();
	} else {
	    QString t = app->type();
	    QPopupMenu* pmenu = typpop.find(t);
	    if ( !pmenu )
		pmenu = menu;
	    pmenu->insertItem( app->pixmap(), app->name(), app->id() );
	    result=TRUE;
	}
    }

    if ( result )
	connect( menu, SIGNAL(activated(int)), SLOT(itemSelected(int)) );

    return result;
}


void StartMenu::launch()
{
    int y = mapToGlobal( QPoint() ).y() - launchMenu->sizeHint().height();

    if ( launchMenu->isVisible() ) 
        launchMenu->hide();
    else
        launchMenu->popup( QPoint( 1, y ) );
}

const AppLnk* StartMenu::execToLink(const QString& appname)
{
    const AppLnk* a = apps->findExec( appname );
    return a;
}

void StartPopupMenu::keyPressEvent( QKeyEvent *e )
{
    if ( e->key() == Key_F33 || e->key() == Key_Space ) {
	// "OK" button, little hacky
	QKeyEvent ke(QEvent::KeyPress, Key_Enter, 13, 0);
	QPopupMenu::keyPressEvent( &ke );
    } else {
	QPopupMenu::keyPressEvent( e );
    }
}
