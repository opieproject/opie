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
#include "sidething.h"
//#include "mrulist.h"
#include "info.h"

#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <qpe/global.h>
#include <qpe/resource.h>
#include <qpe/qlibrary.h>

#include <qintdict.h>
#include <qdir.h>

#include <stdlib.h>


// #define USE_CONFIG_FILE


StartMenu::StartMenu(QWidget *parent) : QLabel( parent )
{
    loadOptions();

    int sz = AppLnk::smallIconSize()+3;
    QPixmap pm;
    pm.convertFromImage(Resource::loadImage(startButtonPixmap).smoothScale(sz,sz));
    setPixmap(pm);
    setFocusPolicy( NoFocus );
    //setFlat( startButtonIsFlat );

    apps = 0;
    launchMenu = 0;
    applets. setAutoDelete ( true );
    sepId = 0;
    
    reloadApps ( );
    reloadApplets ( );
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
    popupMenuSidePixmap = config.readEntry( "PopupMenuSidePixmap", "launcher/sidebar" );
    startButtonPixmap = config.readEntry( "StartButtonPixmap", "launcher/start_button" );
#else
    // Basically just #include the .qpe_menu.conf file settings
    useWidePopupMenu = FALSE;
    popupMenuSidePixmap = "launcher/sidebar";
    startButtonIsFlat = TRUE;
    startButtonPixmap = "launcher/start_button"; // No tr
#endif
}


void StartMenu::createMenu()
{
    delete launchMenu;
    if ( useWidePopupMenu )
        launchMenu = new PopupWithLaunchSideThing( this, &popupMenuSidePixmap );
    else
        launchMenu = new StartPopupMenu( this );

    loadMenu ( apps, launchMenu );
    loadApplets ( );
    
	connect( launchMenu, SIGNAL(activated(int)), SLOT(itemSelected(int)) );
}

void StartMenu::reloadApps()
{
    Config cfg("StartMenu");
    cfg.setGroup("Menu");
    bool ltabs = cfg.readBoolEntry("LauncherTabs",TRUE);
    bool lot = cfg.readBoolEntry("LauncherOther",TRUE);
    bool lt = ltabs || lot;
    if ( launchMenu && apps && !lt )
        return; // nothing to do

    if ( lt ) {
        delete apps;
        apps = new AppLnkSet( QPEApplication::qpeDir() + "apps" );
    }
    if ( launchMenu ) {
    	launchMenu-> hide ( );
    
    	for ( QIntDictIterator<QPopupMenu> it ( tabdict ); it. current ( ); ++it ) {
    		launchMenu-> removeItem ( it. currentKey ( ));
    		delete it.current ( );
    	}
	    tabdict. clear ( );
        loadMenu(apps,launchMenu);
    } else {
        createMenu();
    }
}

void StartMenu::reloadApplets()
{
	if ( launchMenu ) {
	    clearApplets ( );
  	  	loadApplets ( );
 	}
 	else
 		createMenu ( );
}

void StartMenu::itemSelected( int id )
{
    const AppLnk *app = apps->find( id );
    if ( app )
        app->execute();
    else {
    	MenuApplet *applet = applets. find ( id );
    	
    	if ( applet )
    		applet-> iface-> activated ( );
	}   
}

bool StartMenu::loadMenu( AppLnkSet *folder, QPopupMenu *menu )
{
    bool result = FALSE;

    Config cfg("StartMenu");
    cfg.setGroup("Menu");

    bool ltabs = cfg.readBoolEntry("LauncherTabs",TRUE);
    bool lot = cfg.readBoolEntry("LauncherOther",TRUE);
    
    tabdict. clear ( );
    
    if ( sepId )
    	menu-> removeItem ( sepId );
    sepId = ( menu-> count ( )) ? menu-> insertSeparator ( 0 ) : 0;

    if ( ltabs || lot ) {
        QDict<QPopupMenu> typpop;
        QStringList typs = folder->types();
        for (QStringList::Iterator tit=typs.fromLast(); ; --tit) {
            if ( !(*tit).isEmpty() ) {
                QPopupMenu *new_menu;
                if ( ltabs ) {
                    new_menu = new StartPopupMenu( menu );
                    connect( new_menu, SIGNAL(activated(int)), SLOT(itemSelected(int)) );
                    int id = menu->insertItem( folder->typePixmap(*tit), folder->typeName(*tit), new_menu, -1, 0 );
                    tabdict. insert ( id, new_menu );
                } else {
                    new_menu = (QPopupMenu*)1;
                }
                typpop.insert(*tit, new_menu);
            }
            if ( tit == typs. begin ( ))
            	break;
        }
        QListIterator<AppLnk> it( folder->children() );
        bool f=TRUE;
        for ( ; it.current(); ++it ) {
            AppLnk *app = it.current();
            if ( app->type() == "Separator" ) { // No tr
                if ( lot ) {
                    menu->insertSeparator();
                }
            } else {
                f = FALSE;
                QString t = app->type();
                QPopupMenu* pmenu = typpop.find(t);
                if ( ltabs ) {
                    if ( !pmenu && lot )
                        pmenu = menu;
                } else {
                    if ( !pmenu )
                        pmenu = menu;
                    else
                        pmenu = 0;
                }
                if ( pmenu ) {
                    QString t = app->name();
                    t.replace(QRegExp("&"),"&&"); // escape shortcut character
                    pmenu->insertItem( app->pixmap(), t, app->id() );
                }
                result=TRUE;
            }
        }
    }

    if ( sepId && ( menu-> idAt ( 0 ) == sepId )) { // no tabs entries 
    	menu-> removeItem ( sepId );
    	sepId = 0;
	}
	if ( !menu-> count ( )) // if we don't do this QPopupMenu will insert a dummy Separator, which won't go away later
		sepId = menu-> insertSeparator ( );
		
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
    launchMenu-> hide();

    for ( QIntDictIterator<MenuApplet> it ( applets ); it. current ( ); ++it ) {
    	MenuApplet *applet = it. current ( );
    	if ( launchMenu ) {
    		launchMenu-> removeItem ( applet-> id );
    		delete applet-> popup;
    	}
    
        applet-> iface-> release();
        applet-> library-> unload();
        delete applet-> library;
    }
    applets.clear();
}



void StartMenu::loadApplets()
{
    Config cfg( "StartMenu" );
    cfg.setGroup( "Applets" );
    
    // SafeMode causes too much problems, so we disable it for now --
    // maybe we should reenable it for OPIE 1.0 - sandman 26.09.02
    
    bool safe = false; //cfg.readBoolEntry("SafeMode",FALSE);
    if ( safe && !safety_tid )
        return;
    cfg.writeEntry("SafeMode",TRUE);
    cfg.write();
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
    qsort(xapplets,napplets,sizeof(applets[0]),compareAppletPositions);

	if ( sepId )
		launchMenu-> removeItem ( sepId );
	sepId = ( launchMenu-> count ( )) ? launchMenu-> insertSeparator ( ) : 0;
               
    while (napplets--) {
        MenuApplet *applet = xapplets[napplets];
            
        applet-> popup = applet-> iface-> popup ( this );
        
        if ( applet-> popup )
	    	applet-> id = launchMenu-> insertItem ( applet-> iface-> icon ( ), applet-> iface-> text ( ), applet-> popup );
	    else 
	    	applet-> id = launchMenu-> insertItem ( applet-> iface-> icon ( ), applet-> iface-> text ( ) );
        applets.insert ( applet-> id, new MenuApplet(*applet));
    }
    delete xapplets;
    
    if ( sepId && ( launchMenu-> idAt ( launchMenu-> count ( ) - 1 ) == sepId )) { // no applets
    	launchMenu-> removeItem ( sepId );
    	sepId = 0;
    }
	if ( !launchMenu-> count ( )) // if we don't do this QPopupMenu will insert a dummy Separator, which won't go away later
		sepId = launchMenu-> insertSeparator ( );
    
    if ( !safety_tid )
        safety_tid = startTimer(2000); // TT has 5000, but this is a PITA for a developer ;) (sandman)
}

void StartMenu::timerEvent(QTimerEvent* e)
{
    if ( e->timerId() == safety_tid ) {
        Config cfg( "StartMenu" );
        cfg.setGroup( "Applets" );
        cfg.writeEntry( "SafeMode", FALSE );
        killTimer(safety_tid);
        safety_tid = 0;
    }
}

