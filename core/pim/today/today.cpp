/*
 * today.cpp
 *
 * copyright   : (c) 2002,2003,2004 by Maximilian Reiﬂ
 * email       : harlekin@handhelds.org
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "today.h"

#include <opie2/odebug.h>
#include <opie2/opluginloader.h>
#include <opie2/oconfig.h>

#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <qpe/contact.h>

#include <qdir.h>
#include <qtimer.h>
#include <qwhatsthis.h>

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
: TodayBase( parent, name, fl ) {

    QObject::connect( (QObject*)ConfigButton, SIGNAL( clicked() ), this, SLOT( startConfig() ) );
    QObject::connect( (QObject*)OwnerField, SIGNAL( clicked() ), this,  SLOT( editCard() ) );

    #if defined(Q_WS_QWS)
    #if !defined(QT_NO_COP)

    QCopChannel *todayChannel = new QCopChannel( "QPE/Today" , this );
    connect ( todayChannel, SIGNAL( received(const QCString&,const QByteArray&) ),
              this, SLOT ( channelReceived(const QCString&,const QByteArray&) ) );
    #endif
    #endif

    setOwnerField();
    m_big_box = 0L;

    layout = new QVBoxLayout( this );
    layout->addWidget( Frame );
    layout->addWidget( OwnerField );

    m_sv = new QScrollView( this );
    m_sv->setResizePolicy( QScrollView::AutoOneFit );
    m_sv->setHScrollBarMode( QScrollView::AlwaysOff );
    m_sv->setFrameShape( QFrame::NoFrame );

    layout->addWidget( m_sv );
    layout->setStretchFactor( m_sv,4 );

    m_refreshTimer = new QTimer( this );
    connect( m_refreshTimer, SIGNAL( timeout() ), this, SLOT( refresh() ) );

    init();
    loadPlugins();
    initialize();
    QPEApplication::showWidget( this );
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
    QString file = Global::applicationFileName( "addressbook", "businesscard.vcf" );
    if ( QFile::exists( file ) ) {
        Contact cont = Contact::readVCard( file )[0];
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
 * Init stuff needed for today. Reads the config file.
 */
void Today::init() {
    // read config
    OConfig cfg( "today" );
    cfg.setGroup( "Plugins" );
    //     m_excludeApplets = cfg.readListEntry( "ExcludeApplets", ',' );
    //     m_allApplets = cfg.readListEntry( "AllApplets", ',' );

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

    if ( m_big_box ) {
        delete m_big_box;
    }

    m_big_box = new QWidget( m_sv->viewport() );
    m_sv->addChild( m_big_box );
    m_bblayout = new QVBoxLayout ( m_big_box );
}


/**
 * Load the plugins
 */
void Today::loadPlugins() {

    m_pluginLoader = new OPluginLoader( "today", true );
    m_pluginLoader->setAutoDelete( true );

    OPluginItem::List lst = m_pluginLoader->allAvailable( true );

    m_manager = new OPluginManager( m_pluginLoader );
    m_manager->load();

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
        plugPix.convertFromImage( Resource::loadImage( plugin.guiPart->pixmapNameWidget() ).smoothScale( m_iconSize, m_iconSize ), 0 );
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

    if ( conf.exec() == QDialog::Accepted ) {
        conf.writeConfig();
        initialize();
    } else {
        // since reinitialize is not called in that case , reconnect the signal
        m_refreshTimer->start( 15000 ); // get the config value in here later
        connect( m_refreshTimer, SIGNAL( timeout() ), this, SLOT( refresh() ) );
    }
}


void Today::initialize()  {

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

    if ( m_bblayout ) {
    	delete m_bblayout;
    }
    m_bblayout = new QVBoxLayout ( m_big_box );

    if (  pluginList.count() == 0 ) {
        QLabel *noPlugins = new QLabel( this );
        noPlugins->setText( tr( "No plugins found" ) );
        layout->addWidget( noPlugins );
    } else {

        uint count = 0;
        TodayPlugin tempPlugin;
        OPluginItem::List lst = m_pluginLoader->allAvailable( true );
        for ( OPluginItem::List::Iterator it = lst.begin(); it != lst.end(); ++it ) {

            TodayPluginInterface* iface = m_pluginLoader->load<TodayPluginInterface>( *it, IID_TodayPluginInterface );

            tempPlugin = ( pluginList.find( (*it).name() ).data() );
            if ( !( (tempPlugin.name).isEmpty() ) ) {
                if ( (*it).isEnabled() ) {
                    iface->guiPart()->reinitialize();
                    odebug << "reinit" << oendl;
                    tempPlugin.guiBox->show();
		    m_bblayout->addWidget(tempPlugin.guiBox);
                    count++;
                } else {
                    tempPlugin.guiBox->hide();
                }
            }
        }
        if ( count == 0 ) {
            QLabel *noPluginsActive = new QLabel( this );
            noPluginsActive->setText( tr( "No plugins activated" ) );
            layout->addWidget( noPluginsActive );
        }
    }
    m_bblayout->addStretch( 1 );
    repaint();
}

/**
 * Refresh for the view. Reload all applets
 *
 */
void Today::refresh() {

    OPluginItem::List  lst = m_pluginLoader->filtered( true );

    for ( OPluginItem::List::Iterator it = lst.begin(); it != lst.end(); ++it ) {
        TodayPluginInterface* iface = m_pluginLoader->load<TodayPluginInterface>( *it, IID_TodayPluginInterface );
        iface->guiPart()->refresh();
        odebug << "refresh" << oendl;
    }

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
    if (m_pluginLoader) {
        delete m_pluginLoader;
    }
    if (m_manager) {
        delete m_manager;
    }
}

