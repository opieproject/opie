/*
                             This file is part of the Opie Project

                             Copyright (C) Maximilian Reiss <harlekin@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
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

#include "today.h"

#include <opie2/odebug.h>
#include <opie2/opluginloader.h>
#include <opie2/opimcontact.h>
#include <opie2/ocontactaccessbackend_vcard.h>
#include <opie2/ocontactaccess.h>
#include <opie2/oconfig.h>
#include <opie2/oresource.h>

#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>

#include <qdir.h>
#include <qtimer.h>
#include <qwhatsthis.h>
#include <qmessagebox.h>

using namespace Opie::Ui;
using Opie::Core::OPluginItem;
using Opie::Core::OPluginLoader;
using Opie::Core::OPluginManager;
using Opie::Core::OConfig;


struct TodayPlugin {
TodayPlugin() : iface( 0 ), guiPart( 0 ), guiBox( 0 ) {}
    QInterfacePtr<TodayPluginInterface> iface;
    TodayPluginObject *guiPart;
    OPluginItem oplugin;
    QWidget *guiBox;
    QString name;
    bool excludeRefresh;
};

static  QMap<QString, TodayPlugin> pluginList;

Today::Today( QWidget* parent,  const char* name, WFlags fl )
: TodayBase( parent, name, fl | WStyle_ContextHelp) {

    setCaption( tr("Today") );
    connect( (QObject*)ConfigButton, SIGNAL( clicked() ), this, SLOT( startConfig() ) );
    connect( (QObject*)OwnerField, SIGNAL( clicked() ), this,  SLOT( editCard() ) );

#if !defined(QT_NO_COP)

    QCopChannel *todayChannel = new QCopChannel( "QPE/Today" , this );
    connect ( todayChannel, SIGNAL( received(const QCString&,const QByteArray&) ),
              this, SLOT ( channelReceived(const QCString&,const QByteArray&) ) );
#endif

    setOwnerField();
    m_big_box  = 0l;
    m_bblayout = 0l;

    layout = new QVBoxLayout( this );
    layout->addWidget( Frame );
    layout->addWidget( OwnerField );


    m_informationLabel = new QLabel( tr("No plugins activated"),  this );
    layout->addWidget( m_informationLabel );

    m_sv = new QScrollView( this );
    m_sv->setResizePolicy( QScrollView::AutoOneFit );
    m_sv->setHScrollBarMode( QScrollView::AlwaysOff );
    m_sv->setFrameShape( QFrame::NoFrame );

    layout->addWidget( m_sv );
    layout->setStretchFactor( m_sv,4 );

    m_refreshTimer = new QTimer( this );
    connect( m_refreshTimer, SIGNAL( timeout() ), this, SLOT( refresh() ) );

    loadPlugins();
    loadShellContent();
    loadPluginWidgets();
}

/**
 * Qcop receive method.
 */
void Today::channelReceived( const QCString &msg, const QByteArray & data ) {
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "message(QString)" ) {
        QString message;
        stream >> message;
        setOwnerField( message );
    }
}

void Today::setRefreshTimer( int interval ) {

    disconnect( m_refreshTimer, SIGNAL( timeout() ), this, SLOT( refresh() ) );
    // 0 is "never" case
    if ( !interval == 0 ) {
        connect( m_refreshTimer, SIGNAL( timeout() ), this, SLOT( refresh() ) );
        m_refreshTimer->changeInterval( interval );
    }
}


/**
 * Initialises the owner field with the default value, the username
 */
void Today::setOwnerField() {
    QString vfilename = Global::applicationFileName("addressbook", "businesscard.vcf");
    Opie::OPimContactAccess acc( "today", vfilename,
                                 new Opie::OPimContactAccessBackend_VCard("today", vfilename ) );
    if ( acc.load() ) {
        Opie::OPimContact cont = acc.allRecords()[0];
        QString returnString = cont.fullName();
        OwnerField->setText( "<b>" + tr ( "Owned by " ) + returnString + "</b>" );
    } else {
        OwnerField->setText( "<b>" + tr ( "Please fill out the business card" ) + " </b>" );
    }
}

/**
 * Set the owner field with a given QString, for example per qcop.
 */
void Today::setOwnerField( QString &message ) {
    if ( !message.isEmpty() ) {
        OwnerField->setText( "<b>" + message + "</b>" );
    }
}


/**
 * Load the plugins
 */
void Today::loadPlugins() {
    m_pluginLoader = new OPluginLoader( "today", true );
    m_pluginLoader->setAutoDelete( true );

    m_manager = new OPluginManager( m_pluginLoader );
    m_manager->load();
}

void Today::loadShellContent() {
    Config cfg( "today" );
    cfg.setGroup( "Plugins" );


    cfg.setGroup( "General" );
    m_iconSize = cfg.readNumEntry( "IconSize", 18 );
    m_hideBanner = cfg.readNumEntry( "HideBanner", 0 );
    setRefreshTimer( cfg.readNumEntry( "checkinterval", 15000 ) );

    // set the date in top label
    QDate date = QDate::currentDate();
    DateLabel->setText( QString( "<font color=#FFFFFF>" +  TimeString::longDateString( date )  + "</font>" ) );

    if ( m_hideBanner )  {
        Opiezilla->hide();
        TodayLabel->hide();
    } else {
        Opiezilla->show();
        TodayLabel->show();
    }
}

void Today::loadPluginWidgets() {
    /*
     * check if loading of Plugins crashed
     */
    if( m_pluginLoader->isInSafeMode() ) {
        QMessageBox::information(this, tr("Today Error"),
                                 tr("<qt>The plugin '%1' caused Today to crash."
                                    " It could be that the plugin is not properly"
                                    " installed.<br>Today tries to continue loading"
                                    " plugins.</qt>")
                                 .arg( m_manager->crashedPlugin().name()));
    }

    OPluginItem::List lst = m_pluginLoader->filtered( true );

    /*
     * Show or Hide the information of no plugin installed
     */
    if ( lst.isEmpty() )
        m_informationLabel->show();
    else
        m_informationLabel->hide();


    /*
     * Now let us add the plugins
     */
    m_big_box = new QWidget( m_sv->viewport() );
    m_sv->addChild( m_big_box );
    m_bblayout = new QVBoxLayout( m_big_box );

    for ( OPluginItem::List::Iterator it = lst.begin(); it != lst.end(); ++it ) {
        TodayPluginInterface* iface = m_pluginLoader->load<TodayPluginInterface>( *it, IID_TodayPluginInterface );

        TodayPlugin plugin;
        plugin.iface = iface;
        plugin.name = (*it).name();
        plugin.oplugin = (*it);

        plugin.guiPart = plugin.iface->guiPart();
        plugin.excludeRefresh = plugin.guiPart->excludeFromRefresh();

        // package the whole thing into a qwidget so it can be shown and hidden
        plugin.guiBox = new QWidget( m_big_box );
        QHBoxLayout *boxLayout = new QHBoxLayout( plugin.guiBox );
        QPixmap plugPix;
        plugPix.convertFromImage( Opie::Core::OResource::loadImage( plugin.guiPart->pixmapNameWidget() ).smoothScale( m_iconSize, m_iconSize ), 0 );
        OClickableLabel* plugIcon = new OClickableLabel( plugin.guiBox );
        plugIcon->setPixmap( plugPix );
        QWhatsThis::add
            ( plugIcon, tr("Click here to launch the associated app") );
        plugIcon->setName( plugin.guiPart->appName() );
        connect( plugIcon, SIGNAL( clicked() ), this, SLOT( startApplication() ) );

        QWidget *plugWidget = plugin.guiPart->widget( plugin.guiBox );
        boxLayout->addWidget( plugIcon, 0, AlignTop );
        boxLayout->addWidget( plugWidget, 0, AlignTop );
        boxLayout->setStretchFactor( plugIcon, 1 );
        boxLayout->setStretchFactor( plugWidget, 9 );

        pluginList.insert( plugin.name, plugin );
        m_bblayout->addWidget(plugin.guiBox);
    }

    m_bblayout->addStretch( 1 );
    m_big_box->show();
}


/**
 * The method for the configuration dialog.
 */
void Today::startConfig() {
    // disconnect timer to prevent problems while being on config dialog
    disconnect( m_refreshTimer, SIGNAL( timeout() ), this, SLOT( refresh() ) );
    m_refreshTimer->stop( );

    TodayConfig conf( this, "dialog", true );
    conf.setUpPlugins( m_manager, m_pluginLoader );

    if ( QPEApplication::execDialog(&conf) == QDialog::Accepted ) {
        conf.writeConfig();
        clearPluginWidgets();
        loadShellContent();
        loadPluginWidgets();
    } else {
        // since reinitialize is not called in that case , reconnect the signal
        m_refreshTimer->start( 15000 ); // get the config value in here later
        connect( m_refreshTimer, SIGNAL( timeout() ), this, SLOT( refresh() ) );
    }
}

/**
 * Refresh for the view. Reload all applets
 *
 */
void Today::refresh() {
    for ( QMap<QString, TodayPlugin>::Iterator it = pluginList.begin();
          it != pluginList.end(); ++it )
        it.data().guiPart->refresh();

    DateLabel->setText( QString( "<font color=#FFFFFF>" +  TimeString::longDateString( QDate::currentDate() )  + "</font>" ) );

    updateGeometry();
    repaint();
}


void Today::startApplication() {
    QCopEnvelope e( "QPE/System", "execute(QString)" );
    e << QString( sender()->name() );
}


/**
* launch addressbook (personal card)
*/
void Today::editCard() {
    QCopEnvelope env( "QPE/Application/addressbook", "editPersonalAndClose()" );
}


Today::~Today() {
    clearPluginWidgets();
    delete m_pluginLoader;
    delete m_manager;
}


void Today::clearPluginWidgets() {
    for(QMap<QString, TodayPlugin>::Iterator it = pluginList.begin(); it != pluginList.end(); ++it ) {
        delete it.data().guiBox;
        it.data().guiBox = 0;
    }

    pluginList.clear();

    delete m_bblayout;
    delete m_big_box;
    m_bblayout = 0;
    m_big_box  = 0;
}
