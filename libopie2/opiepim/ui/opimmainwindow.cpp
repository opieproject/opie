/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "opimmainwindow.h"

/* OPIE */
#include <opie2/opimresolver.h>
#include <opie2/odebug.h>

#include <qpe/categoryselect.h>
#include <qpe/ir.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/sound.h>

/* QT */
#include <qaction.h>
#include <qapplication.h>
#include <qcopchannel_qws.h>
#include <qdatetime.h>
#include <qmenubar.h>
#include <qobjectlist.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>

namespace Opie {
OPimMainWindow::OPimMainWindow( const QString &serviceName,
                                const QString &appName, const QString &catName,
                                const QString &itemName, const QString &configName,
                                QWidget *parent, const char* name, WFlags f )
    : QMainWindow( parent, name, f ), m_rtti(-1), m_service( serviceName ), m_fallBack( 0l ),
      m_appName( appName ), m_catGroupName( catName ), m_config( configName ), m_itemContextMenu( 0l )
{

    /*
     * let's generate our QCopChannel
     */
    m_str = QString("QPE/"+m_service).local8Bit();
    m_channel= new QCopChannel(m_str, this );
    connect(m_channel, SIGNAL(received(const QCString&,const QByteArray&)),
            this, SLOT(appMessage(const QCString&,const QByteArray&)) );
    connect(qApp, SIGNAL(appMessage(const QCString&,const QByteArray&)),
            this, SLOT(appMessage(const QCString&,const QByteArray&)) );

    /* connect flush and reload */
    connect(qApp, SIGNAL(flush() ),
            this, SLOT(flush() ) );
    connect(qApp, SIGNAL(reload() ),
            this, SLOT(reload() ) );

    // Initialize user interface items
    setCaption( m_appName );
    initBars( itemName );
}

OPimMainWindow::~OPimMainWindow() {
    delete m_channel;
}

QCopChannel* OPimMainWindow::channel() {
    return m_channel;
}

void OPimMainWindow::doSetDocument( const QString&  ) {

}

void OPimMainWindow::appMessage( const QCString& cmd, const QByteArray& array ) {
    bool needShow = false;
    /*
     * create demands to create
     * a new record...
     */
    QDataStream stream(array, IO_ReadOnly);
    if ( cmd == "create()" ) {
        raise();
        int uid = create();
        QCopEnvelope e(m_str, "created(int)" );
        e << uid;
        needShow = true;
    }else if ( cmd == "remove(int)" ) {
        int uid;
        stream >> uid;
        bool rem = remove( uid );
        QCopEnvelope e(m_str, "removed(bool)" );
        e << rem;
        needShow = true;
    }else if ( cmd == "beam(int)" ) {
        int uid;
        stream >> uid;
        beam( uid);
    }else if ( cmd == "show(int)" ) {
        raise();
        int uid;
        stream >> uid;
        show( uid );
        needShow = true;
    }else if ( cmd == "edit(int)" ) {
        raise();
        int uid;
        stream >> uid;
        edit( uid );
    }else if ( cmd == "add(int,QByteArray)" ) {
        int rtti;
        QByteArray array;
        stream >> rtti;
        stream >> array;
        m_fallBack = record(rtti, array );
        if (!m_fallBack) return;
        add( *m_fallBack );
        delete m_fallBack;
    }else if ( cmd == "alarm(QDateTime,int)" ) {
        raise();
        QDateTime dt; int uid;
        stream >> dt;
        stream >> uid;
        QDateTime current = QDateTime::currentDateTime();
        if ( current.time().hour() != dt.time().hour() && current.time().minute() != dt.time().minute() )
            return;
        doAlarm( dt,  uid );
        needShow = true;
    }

    if (needShow )
        QPEApplication::setKeepRunning();
}

/* implement the url scripting here */
void OPimMainWindow::setDocument( const QString& str) {
    doSetDocument( str );
}

/*
 * we now try to get the array demarshalled
 * check if the rtti matches this one
 */
OPimRecord* OPimMainWindow::record( int rtti,  const QByteArray& array ) {
    if ( service() != rtti )
        return 0l;

    OPimRecord* record = OPimResolver::self()->record( rtti );
    QDataStream str(array, IO_ReadOnly );
    if ( !record || !record->loadFromStream(str) ) {
        delete record;
        record = 0l;
    }

    return record;
}

/*
 * get the rtti for the service
 */
int OPimMainWindow::service() {
    if ( m_rtti == -1 )
        m_rtti  =  OPimResolver::self()->serviceId( m_service );

    return m_rtti;
}

void OPimMainWindow::doAlarm( const QDateTime&, int  ) {

}

void OPimMainWindow::startAlarm(int count  ) {
    m_alarmCount = count;
    m_playedCount = 0;
    Sound::soundAlarm();
    m_timerId = startTimer( 5000 );
}

void OPimMainWindow::killAlarm() {
    killTimer( m_timerId );
}

void OPimMainWindow::timerEvent( QTimerEvent* e) {
    if ( m_playedCount <m_alarmCount ) {
        m_playedCount++;
        Sound::soundAlarm();
    }else {
        killTimer( e->timerId() );
    }
}

QPopupMenu *OPimMainWindow::itemContextMenu() {
    if ( !m_itemContextMenu ) {
        // Create context menu if hasn't been done before
        m_itemContextMenu = new QPopupMenu( this );
        m_itemEditAction->addTo( m_itemContextMenu );
        m_itemDuplicateAction->addTo( m_itemContextMenu );
        m_itemDeleteAction->addTo( m_itemContextMenu );
    }

    return m_itemContextMenu;
}

void OPimMainWindow::insertItemMenuItems( QActionGroup *items ) {
    // Insert menu items into Item menu
    if ( items ) {
        // Rebuild Item menu
        m_itemMenu->clear();
        m_itemMenuGroup1->addTo( m_itemMenu );
        m_itemMenu->insertSeparator();
        items->addTo( m_itemMenu );
        m_itemMenu->insertSeparator();
        m_itemMenuGroup2->addTo( m_itemMenu );
    }
}

void OPimMainWindow::insertViewMenuItems( QActionGroup *items ) {
    // Insert menu items into View menu
    if ( items ) {
        // Rebuild Item menu
        m_viewMenu->clear();
        m_viewMenuCategories->addTo( m_viewMenu );
        m_viewMenu->insertSeparator();
        m_viewMenuGroup->addTo( m_viewMenu );
        m_viewMenu->insertSeparator();
        m_viewMenuAppGroup = items;
        m_viewMenuAppGroup->addTo( m_viewMenu );
    }
}

void OPimMainWindow::slotViewCategory( QAction *category ) {
    // Set application caption
    QString caption = m_appName;
    if ( category->text() != tr( "All" ) )
        caption.append( QString( " - %1" ).arg( category->text() ) );
    setCaption( caption );

    // Notify application
    emit categorySelected( category->text() );
}

void OPimMainWindow::setViewCategory( const QString &category ) {
        // Find category in menu
        QObjectListIt kidIt( *(m_viewMenuCategories->children()) );
        QObject *obj;
        while ( (obj=kidIt.current()) != 0 ) {
            QAction *currAction = reinterpret_cast<QAction*>(obj);
            if ( currAction->text() == category ) {
                // Category was found, enable it
                currAction->setOn( true );
                return;
            }
            ++kidIt;
        }
}

void OPimMainWindow::reloadCategories() {
    QString selected;

    // Remove old categories from View menu
    if ( m_viewMenuCategories ) {
        QObjectListIt kidIt( *(m_viewMenuCategories->children()) );
        QObject *obj;
        while ( (obj=kidIt.current()) != 0 ) {
            QAction *currAction = reinterpret_cast<QAction*>(obj);
            if ( currAction->isOn() )
                selected = currAction->text();
            ++kidIt;
            delete currAction;
        }
    }
    else {
        m_viewMenuCategories = new QActionGroup( this );
        connect( m_viewMenuCategories, SIGNAL(selected(QAction*)), this, SLOT(slotViewCategory(QAction*)) );

        selected = tr( "All" );
    }

    m_viewMenu->clear();
    
    // Add categories to View menu
    QAction *a = new QAction( tr( "All" ), QString::null, 0, m_viewMenuCategories, QString::null, true );
    a->setWhatsThis( tr( "Click here to view all items." ) );
    a->setOn( selected == tr( "All" ) );

    Categories cats;
    cats.load( categoryFileName() );
    QStringList catList = cats.labels( m_catGroupName );
    for ( QStringList::Iterator it = catList.begin(); it != catList.end(); ++it ) {
        a = new QAction( tr( (*it) ), QString::null, 0, m_viewMenuCategories, QString::null, true );
        a->setWhatsThis( tr( "Click here to view items belonging to %1." ).arg( (*it) ) );
        a->setOn( selected == (*it) );
    }
    
    a = new QAction( tr( "Unfiled" ), QString::null, 0, m_viewMenuCategories, QString::null, true );
    a->setWhatsThis( tr( "Click here to view all unfiled items." ) );
    a->setOn( selected == tr( "Unfiled" ) );
    
    m_viewMenuCategories->addTo( m_viewMenu );

    // Add default items to View menu
    m_viewMenu->insertSeparator();
    m_viewMenuGroup->addTo( m_viewMenu );

    // Insert application-specific items (if any)
    if ( m_viewMenuAppGroup ) {
        m_viewMenu->insertSeparator();
        m_viewMenuAppGroup->addTo( m_viewMenu );
    }
}

void OPimMainWindow::initBars( const QString &itemName ) {
    QString itemStr = itemName.lower();

    setToolBarsMovable( false );

    // Create application menu bar
    QToolBar *mbHold = new QToolBar( this );
    mbHold->setHorizontalStretchable( true );
    QMenuBar *menubar = new QMenuBar( mbHold );
    menubar->setMargin( 0 );

    // Create application menu bar
    QToolBar *toolbar = new QToolBar( this );
    
    // Create sub-menus
    m_itemMenu = new QPopupMenu( this );
    m_itemMenu->setCheckable( true );
    menubar->insertItem( itemName, m_itemMenu );
    m_viewMenu = new QPopupMenu( this );
    m_viewMenu->setCheckable( true );
    menubar->insertItem( tr( "View" ), m_viewMenu );

    m_viewMenuCategories = 0l;
    m_viewMenuAppGroup = 0l;
    
    // Item menu
    m_itemMenuGroup1 = new QActionGroup( this, QString::null, false );
    
    QAction *a = new QAction( tr( "New" ), Resource::loadPixmap( "new" ),
                              QString::null, 0, m_itemMenuGroup1, 0 );
    connect( a, SIGNAL(activated()), this, SLOT(slotItemNew()) );
    a->setWhatsThis( tr( "Click here to create a new item." ) );
    a->addTo( toolbar );

    m_itemEditAction = new QAction( tr( "Edit" ), Resource::loadPixmap( "edit" ),
                                    QString::null, 0, m_itemMenuGroup1, 0 );
    connect( m_itemEditAction, SIGNAL(activated()), this, SLOT(slotItemEdit()) );
    m_itemEditAction->setWhatsThis( tr( "Click here to edit the selected item." ) );
    m_itemEditAction->addTo( toolbar );

    m_itemDuplicateAction = new QAction( tr( "Duplicate" ), QString::null, 0, m_itemMenuGroup1, 0 );
    connect( m_itemDuplicateAction, SIGNAL(activated()), this, SLOT(slotItemDuplicate()) );
    m_itemDuplicateAction->setWhatsThis( tr( "Click here to duplicate the selected item." ) );
    
    m_itemDeleteAction = new QAction( tr( "Delete" ), Resource::loadPixmap( "trash" ),
                                      QString::null, 0, m_itemMenuGroup1, 0 );
    connect( m_itemDeleteAction, SIGNAL(activated()), this, SLOT(slotItemDelete()) );
    m_itemDeleteAction->setWhatsThis( tr( "Click here to delete the selected item." ) );
    m_itemDeleteAction->addTo( toolbar );

    if ( Ir::supported() ) {
        a = new QAction( tr( "Beam" ), Resource::loadPixmap( "beam" ),
                         QString::null, 0, m_itemMenuGroup1, 0 );
        connect( a, SIGNAL(activated()), this, SLOT(slotItemBeam()) );
        a->setWhatsThis( tr( "Click here to transmit the selected item." ) );
        //a->addTo( m_itemMenu );
        a->addTo( toolbar );
    }

    m_itemMenuGroup1->addTo( m_itemMenu );

    m_itemMenu->insertSeparator();

    m_itemMenuGroup2 = new QActionGroup( this, QString::null, false );

    a = new QAction( tr( "Find" ), Resource::loadPixmap( "find" ),
                     QString::null, 0, m_itemMenuGroup2, 0 );
    connect( a, SIGNAL(activated()), this, SLOT(slotItemFind()) );
    a->setWhatsThis( tr( "Click here to search for an item." ) );
    a->addTo( toolbar );

    a = new QAction( tr( "Configure" ), Resource::loadPixmap( "SettingsIcon" ),
                     QString::null, 0, m_itemMenuGroup2, 0 );
    connect( a, SIGNAL(activated()), this, SLOT(slotConfigure()) );
    a->setWhatsThis( tr( "Click here to set your preferences for this application." ) );

    m_itemMenuGroup2->addTo( m_itemMenu );
    
    // View menu
    m_viewMenuGroup = new QActionGroup( this, QString::null, false );
    
    a = new QAction( tr( "Filter" ), QString::null, 0, m_viewMenuGroup, 0 );
    connect( a, SIGNAL(activated()), this, SLOT(slotViewFilter()) );
    a->setWhatsThis( tr( "Click here to filter the items displayed." ) );
    
    a = new QAction( tr( "Filter Settings" ), QString::null, 0, m_viewMenuGroup, 0 );
    connect( a, SIGNAL(activated()), this, SLOT(slotViewFilterSettings()) );
    a->setWhatsThis( tr( "Click here to modify the current filter settings." ) );

    reloadCategories();
}

} // namespace Opie
