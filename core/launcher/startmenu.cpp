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

#define INCLUDE_MENUITEM_DEF

#include "startmenu.h"

#include <qtopia/qpeapplication.h>
#include <qtopia/config.h>
#include <qtopia/applnk.h>
#include <qtopia/global.h>
#include <qtopia/resource.h>
#include <qtopia/mimetype.h>

#include <qdict.h>
#include <qdir.h>
#include <qpainter.h>

#include <stdlib.h>


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

//---------------------------------------------------------------------------

StartMenu::StartMenu(QWidget *parent) : QLabel( parent )
{
    startButtonPixmap = "go"; // No tr

    int sz = AppLnk::smallIconSize()+3;
    QPixmap pm;
    pm.convertFromImage(Resource::loadImage(startButtonPixmap).smoothScale(sz,sz));
    setPixmap(pm);
    setFocusPolicy( NoFocus );

    launchMenu = 0;
    refreshMenu();
}


void StartMenu::mousePressEvent( QMouseEvent * )
{
    launch();
}


StartMenu::~StartMenu()
{
}

void StartMenu::createMenu()
{
    delete launchMenu;
    launchMenu = new StartPopupMenu( this );
    loadMenu( launchMenu );
}

void StartMenu::refreshMenu()
{
    Config cfg("Taskbar");
    cfg.setGroup("Menu");
    bool ltabs = cfg.readBoolEntry("LauncherTabs",TRUE);
    bool lot = cfg.readBoolEntry("LauncherOther",TRUE);
    bool lt = ltabs || lot;
    if ( launchMenu && !lt )
	return; // nothing to do

    if ( launchMenu ) {
	int i;
	for (i=0; i<(int)launchMenu->count(); i++) {
	    QMenuItem* item = launchMenu->findItem(launchMenu->idAt(i));
	    if ( item && item->id() >= 0 && item->id() < ntabs ) {
		break;
	    }
	    if ( item && item->isSeparator() ) {
		i++;
		break;
	    }
	}
	while (i<(int)launchMenu->count())
	    launchMenu->removeItemAt(i);
	loadMenu(launchMenu);
    } else {
	createMenu();
    }
}

void StartMenu::itemSelected( int id )
{
    if ( id >= 0 && id < ntabs ) {
	emit tabSelected(tabs[id]);
    } else if ( id >= 20 && id < 20+nother ) {
	other.at(id-20)->execute();
    }
}

bool StartMenu::loadMenu( QPopupMenu *menu )
{
    Config cfg("Taskbar");
    cfg.setGroup("Menu");

    bool ltabs = cfg.readBoolEntry("LauncherTabs",TRUE);
    bool lot = cfg.readBoolEntry("LauncherOther",TRUE);
    bool sepfirst = !ltabs && !lot;

    tabs.clear();
    other.setAutoDelete(TRUE);
    other.clear();
    ntabs = 0;
    nother = 0;

    bool f=TRUE;
    if ( ltabs || lot ) {
	QDir dir( MimeType::appsFolderName(), QString::null, QDir::Name );
	for (int i=0; i<(int)dir.count(); i++) {
	    QString d = dir[i];
	    Config cfg(dir.path()+"/"+d+"/.directory",Config::File);
	    if ( cfg.isValid() ) {
		QString nm = cfg.readEntry("Name");
		QString ic = cfg.readEntry("Icon");
		if ( !!nm && !!ic ) {
		    tabs.append(d);
		    menu->insertItem( Resource::loadIconSet(ic), nm, ntabs++ );
		}
	    } else if ( lot && d.right(8)==".desktop") {
		AppLnk* applnk = new AppLnk(dir.path()+"/"+d);
		if ( applnk->isValid() ) {
		    if ( applnk->type() == "Separator" ) { // No tr
			if ( lot ) {
			    menu->insertSeparator();
			    sepfirst = f && !ltabs;
			}
			delete applnk;
		    } else {
			f = FALSE;
			other.append(applnk);
			menu->insertItem( Resource::loadIconSet(applnk->icon()),
				applnk->name(), 20+nother++ );
		    }
		} else {
		    delete applnk;
		}
	    }
	}

	if ( !menu->count() )
	    sepfirst = TRUE;
    }

    launchMenu->setName(sepfirst ? "accessories" : "accessories_need_sep"); // No tr

    bool result = nother || ntabs;
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

