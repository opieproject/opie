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
#include <qtopia/qlibrary.h>

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
    pm.convertFromImage(Resource::loadImage( startButtonPixmap).smoothScale( sz,sz) );
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
    clearApplets();
}

void StartMenu::createMenu()
{
    clearApplets();
    delete launchMenu;

    launchMenu = new StartPopupMenu( this );
    loadMenu( launchMenu );
    loadApplets();

    bool result = nother || ntabs || m_applets.count();
    if ( result )
	connect( launchMenu, SIGNAL(activated(int)), SLOT(itemSelected(int)) );

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
        /* find the first entry we want to remove */
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
        /* remove them */
	while (i<(int)launchMenu->count())
	    launchMenu->removeItemAt(i);
	loadMenu(launchMenu);
        addApplets(launchMenu);
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
    }else {
        MenuApplet *applet = m_applets.find ( id );
        if ( applet ) {
            qWarning("activated");
            applet-> iface-> activated();
        }
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
                    QPixmap test;
                    test.convertFromImage( Resource::loadImage( ic ).smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() ), 0 );
		    menu->insertItem( test, nm, ntabs++ );
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
                        QPixmap test;
                        test.convertFromImage( Resource::loadImage( applnk->icon() ).smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() ), 0 );
                        menu->insertItem( test  , applnk->name(), 20+nother++ );
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

    return (nother || ntabs );
}


void StartMenu::launch()
{
    int y = mapToGlobal( QPoint() ).y() - launchMenu->sizeHint().height();

    if ( launchMenu->isVisible() )
        launchMenu->hide();
    else
        launchMenu->popup( QPoint( 1, y ) );
}




static int compareAppletPositions(const void *a, const void *b)
{
    const MenuApplet* aa = *(const MenuApplet**)a;
    const MenuApplet* ab = *(const MenuApplet**)b;
    int d = aa->iface->position() - ab->iface->position();
    if ( d ) return d;
    return QString::compare(aa->library->library(),ab->library->library());
}

void StartMenu::clearApplets()
{
    if (launchMenu )
        launchMenu-> hide();

    for ( QIntDictIterator<MenuApplet> it ( m_applets ); it. current ( ); ++it ) {
    	MenuApplet *applet = it. current ( );
    	if ( launchMenu ) {
    		launchMenu-> removeItem ( applet-> id );
    		delete applet-> popup;
    	}

        applet-> iface-> release();
        applet-> library-> unload();
        delete applet-> library;
    }
    m_applets.clear();
}




void StartMenu::loadApplets()
{
    Config cfg( "StartMenu" );
    cfg.setGroup( "Applets" );

    // SafeMode causes too much problems, so we disable it for now --
    // maybe we should reenable it for OPIE 1.0 - sandman 26.09.02
    // removed in the remerge PluginManager could handle it
    // we don't currently use it -zecke

    QStringList exclude = cfg.readListEntry( "ExcludeApplets", ',' );

    QString lang = getenv( "LANG" );
    QString path = QPEApplication::qpeDir() + "/plugins/applets";
    QDir dir( path, "lib*.so" );
    QStringList list = dir.entryList();
    QStringList::Iterator it;
    int napplets=0;
    MenuApplet* *xapplets = new MenuApplet*[list.count()];
    for ( it = list.begin(); it != list.end(); ++it ) {
        if ( exclude.find( *it ) != exclude.end() )
            continue;
        MenuAppletInterface *iface = 0;
        QLibrary *lib = new QLibrary( path + "/" + *it );
        if (( lib->queryInterface( IID_MenuApplet, (QUnknownInterface**)&iface ) == QS_OK ) && iface ) {
            MenuApplet *applet = new MenuApplet;
            xapplets[napplets++] = applet;
            applet->library = lib;
            applet->iface = iface;

            QTranslator *trans = new QTranslator(qApp);
            QString type = (*it).left( (*it).find(".") );
            QString tfn = QPEApplication::qpeDir()+"/i18n/"+lang+"/"+type+".qm";
            if ( trans->load( tfn ))
                qApp->installTranslator( trans );
            else
                delete trans;
        } else {
            exclude += *it;
            delete lib;
        }
    }
    cfg.writeEntry( "ExcludeApplets", exclude, ',' );
    qsort(xapplets,napplets,sizeof(m_applets[0]),compareAppletPositions);


    int foo = ( launchMenu-> count ( )) ? launchMenu-> insertSeparator ( ) : 0;

    while (napplets--) {
        MenuApplet *applet = xapplets[napplets];

        applet-> popup = applet-> iface-> popup ( this );

        if ( applet-> popup )
	    	applet-> id = launchMenu-> insertItem ( applet-> iface-> icon ( ), applet-> iface-> text ( ), applet-> popup );
        else
	    	applet-> id = launchMenu-> insertItem ( applet-> iface-> icon ( ), applet-> iface-> text ( ) );


        m_applets.insert ( applet-> id, new MenuApplet(*applet));
    }
    delete [] xapplets;

}


/*
 * Launcher calls loadMenu too often fix that
 */
void StartMenu::addApplets(QPopupMenu* pop) {
    QIntDict<MenuApplet> dict;
    if( pop-> count ( ))
        pop-> insertSeparator ( );

    for ( QIntDictIterator<MenuApplet> it ( m_applets ); it. current ( ); ++it ) {
    	MenuApplet *applet = it. current ( );
        if ( applet-> popup )
            applet-> id = pop-> insertItem ( applet-> iface-> icon ( ), applet-> iface-> text ( ), applet-> popup );
        else
            applet-> id = pop-> insertItem ( applet-> iface-> icon ( ), applet-> iface-> text ( ) );

        dict.insert( applet->id, new MenuApplet(*applet) );
    }
    /* need to update the key */
    m_applets.setAutoDelete( true );
    m_applets.clear();
    m_applets.setAutoDelete( false );
    m_applets = dict;
}
