/*
                             This file is part of the Opie Project
              =.             (C) 2000-2002 Trolltech AS
            .=l.             (C) 2002-2005 The Opie Team <opie-devel@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
    :`=1 )Y*s>-.--   :       the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
    :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/
// TODO. During startup
// Launcher::typeAdded
// is called for each new tab and calls then each time the refresh of startmenu
// suboptimal

#define INCLUDE_MENUITEM_DEF

#include "startmenu.h"

/* OPIE */
#include <opie2/oresource.h>
using namespace Opie::Core;
#include <qtopia/qpeapplication.h>
#include <qtopia/config.h>
#include <qtopia/mimetype.h>
#include <qtopia/qlibrary.h>

#define APPLNK_ID_OFFSET 250
#define NO_ID -1


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
    pm.convertFromImage(OResource::loadImage( startButtonPixmap, OResource::NoScale ).smoothScale( sz,sz) );
    setPixmap(pm);
    setFocusPolicy( NoFocus );

    useWidePopupMenu = true;
    launchMenu = 0;
    currentItem = 0;
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

    bool result = currentItem || menuApplets.count();
    if ( result )
	    connect( launchMenu, SIGNAL(activated(int)), SLOT(itemSelected(int)) );
}

void StartMenu::refreshMenu()
{
    Config cfg( "StartMenu" );
    cfg.setGroup( "Menu" );
    bool ltabs = cfg.readBoolEntry( "LauncherTabs", TRUE );
    bool lot = cfg.readBoolEntry( "LauncherOther", TRUE );
    useWidePopupMenu = cfg.readBoolEntry(  "LauncherSubPopup",  TRUE );

    if ( launchMenu && !(ltabs || lot) ) return; // nothing to do

	createMenu();
}

void StartMenu::itemSelected( int id )
{
    if ( id == NO_ID ) return;

    if ( id < 0 ) {
        MenuApplet *applet = menuApplets.find( id );
        if ( applet ) {
            applet->iface->activated();
        }
    } else if ( id >= APPLNK_ID_OFFSET ) {
    	AppLnk * appLnk = appLnks.find( id );
        if ( appLnk ) {
		    appLnk->execute();
        }
    } else {
    	QString *tabName = tabNames.find( id );
        if ( tabName ) {
    		emit tabSelected( *tabName );
	    }
    }
}

void StartMenu::createAppEntry( QPopupMenu *menu, QDir dir, QString file )
{
    if ( file.right(8) == ".desktop" ) {
   		AppLnk* applnk = new AppLnk( dir.path() + "/" + file );
    	if ( !applnk->isValid() ) {
            delete applnk;
            return;
        }

	    if ( applnk->type() == "Separator" ) { // No tr
		    menu->insertSeparator();
    		delete applnk;
       	} else {
            QPixmap pixmap = OResource::loadPixmap( applnk->icon(), OResource::SmallIcon );
                // Insert items ordered lexically
		int current, left = 0, right = currentItem;
                while( left != right ) {
                    current = ( left + right ) / 2;
                    if ( menu->text(menu->idAt( ( current ) ) ) < applnk->name() )
                        left = ++current;
                    else
                        right = current;
                }

                menu->insertItem( pixmap, applnk->name(),
                              currentItem + APPLNK_ID_OFFSET, current );
                appLnks.insert( currentItem + APPLNK_ID_OFFSET, applnk );
                currentItem++;
   		}
    }

}

void StartMenu::createDirEntry( QPopupMenu *menu, QDir dir, QString file, bool lot )
{
    // do some sanity checks and collect information

    if ( file == "." || file == ".." ) return;

    Config cfg( dir.path() + "/" + file + "/.directory", Config::File );
    if ( !cfg.isValid() ) return;

   	QString name = cfg.readEntry( "Name" );
    QString icon = cfg.readEntry( "Icon" );
	if ( !name || !icon ) return;

    QDir subdir = QDir( dir );
    subdir.cd( file );
    subdir.setFilter( QDir::Files );
    subdir.setNameFilter( "*.desktop" );
    // we don' t show the menu if there are no entries
    // perhaps one should check if there exist subsubdirs with entries...
    if ( subdir.entryList().isEmpty() ) return;

    // checks were ok

    QPixmap pixmap = OResource::loadPixmap( icon, OResource::SmallIcon );
	if ( useWidePopupMenu ) {
        // generate submenu
        QPopupMenu *submenu = new QPopupMenu( menu );
        connect( submenu,  SIGNAL(activated(int)), SLOT(itemSelected(int)) );
        menu->insertItem( pixmap, name, submenu, NO_ID );

        // ltabs is true cause else we wouldn't stuck around..
        createMenuEntries( submenu, subdir, true, lot );
    } else {
        // no submenus - just bring corresponding tab to front
        menu->insertItem( pixmap, name, currentItem );
        tabNames.insert( currentItem, new QString( file ) );
        currentItem++;
    }
}

void StartMenu::createMenuEntries( QPopupMenu *menu, QDir dir, bool ltabs, bool lot )
{
    if ( lot ) {
        dir.setFilter( QDir::Files );
        dir.setNameFilter( "*.desktop" );
        QStringList files = dir.entryList();
        files.sort();

        for ( QStringList::Iterator it = files.begin(); it != files.end(); it++ ) {
            createAppEntry( menu, dir, *it );
        }
    }
    if ( ltabs ) {
        dir.setNameFilter( "*" );
        dir.setFilter( QDir::Dirs );
        QStringList dirs = dir.entryList();
        dirs.sort();

        for ( QStringList::Iterator it = dirs.begin(); it != dirs.end(); it++ ) {
            createDirEntry( menu, dir, *it, lot );
    	}
    }
}

bool StartMenu::loadMenu( QPopupMenu *menu )
{
    Config cfg("StartMenu");
    cfg.setGroup("Menu");

    bool ltabs = cfg.readBoolEntry("LauncherTabs", TRUE);
    bool lot = cfg.readBoolEntry("LauncherOther", TRUE);
    useWidePopupMenu = cfg.readBoolEntry(  "LauncherSubPopup",  TRUE );
    bool sepfirst = !ltabs && !lot;

    currentItem = 0;
	launchMenu->clear();

    appLnks.setAutoDelete( true );
    tabNames.setAutoDelete( true );
    appLnks.clear();
    tabNames.clear();
    appLnks.setAutoDelete( false );
    tabNames.setAutoDelete( false );

    QDir dir( MimeType::appsFolderName(), QString::null, QDir::Name );
    createMenuEntries( menu, dir, ltabs, lot );

   	if ( !menu->count() ) sepfirst = TRUE;

    launchMenu->setName( sepfirst ? "accessories" : "accessories_need_sep" ); // No tr

    return currentItem;
}


void StartMenu::launch()
{
    int y = mapToGlobal( QPoint() ).y() - launchMenu->sizeHint().height();

    if ( launchMenu->isVisible() )
        launchMenu->hide();
    else
        launchMenu->popup( QPoint( 0, y ) );
}




static int compareAppletPositions(const void *b, const void *a)
{
    const MenuApplet* aa = *(const MenuApplet**)a;
    const MenuApplet* ab = *(const MenuApplet**)b;
    int d = aa->iface->position() - ab->iface->position();
    if ( d ) return d;
    return QString::compare(aa->library->library(),ab->library->library());
}

void StartMenu::clearApplets()
{
    if ( launchMenu )
        launchMenu-> hide();

    for ( QIntDictIterator<MenuApplet> it( menuApplets ); it.current(); ++it ) {
    	MenuApplet *applet = it.current();
    	if ( launchMenu ) {
    		launchMenu->removeItem( applet-> id );
    		delete applet->popup;
    	}

        applet->iface->release();
        applet->library->unload();
        delete applet-> library;
    }
    menuApplets.clear();
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
    QString path = QPEApplication::qpeDir() + "plugins/applets";
    QDir dir( path, "lib*.so" );
    QStringList list = dir.entryList();
    QStringList::Iterator it;
    int napplets = 0;
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
            QString tfn = QPEApplication::qpeDir()+"i18n/"+lang+"/"+type+".qm";
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
    qsort(xapplets, napplets, sizeof(menuApplets[0]), compareAppletPositions);


    while ( napplets-- ) {
        MenuApplet *applet = xapplets[napplets];
        applet->popup = applet->iface->popup( this );

        // menuApplets got an id < -1
        menuApplets.insert( -( currentItem + 2 ), new MenuApplet( *applet ) );
        currentItem++;
    }
    delete [] xapplets;

    addApplets( launchMenu );
}


/*
 * Launcher calls loadMenu too often fix that
 */
void StartMenu::addApplets(QPopupMenu* pop) {
    QIntDict<MenuApplet> dict;
    if( pop-> count ( ))
        pop-> insertSeparator ( );

    for ( QIntDictIterator<MenuApplet> it( menuApplets ); it.current(); ++it ) {
    	MenuApplet *applet = it.current();
        if ( applet->popup )
            applet->id = pop->insertItem( applet->iface->icon(),
                                          applet->iface->text(), applet->popup );
        else
            applet->id = pop->insertItem( applet->iface->icon(),
                                          applet->iface->text() );


        dict.insert( applet->id, new MenuApplet( *applet ) );
    }
    /* need to update the key */
    menuApplets.setAutoDelete( true );
    menuApplets.clear();
    menuApplets.setAutoDelete( false );
    menuApplets = dict;
}
