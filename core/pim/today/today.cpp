/*
 * today.cpp
 *
 * copyright   : (c) 2002 by Maximilian Reiﬂ
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
#include <opie/todayconfigwidget.h>

#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>
#include <qpe/global.h>
#include <qpe/qpeapplication.h>
#include <qpe/contact.h>

#include <qdir.h>
#include <qfile.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qhbox.h>
#include <opie/otabwidget.h>
#include <qdialog.h>
#include <qwhatsthis.h>


struct TodayPlugin {
    QLibrary *library;
    TodayPluginInterface *iface;
    TodayPluginObject *guiPart;
    QWidget *guiBox;
    QString name;
    bool active;
    bool excludeRefresh;
    int pos;
};

static QValueList<TodayPlugin> pluginList;

Today::Today( QWidget* parent,  const char* name, WFlags fl )
    : TodayBase( parent, name, fl ) {

    QObject::connect( (QObject*)ConfigButton, SIGNAL( clicked() ), this, SLOT( startConfig() ) );
    QObject::connect( (QObject*)OwnerField, SIGNAL( clicked() ), this,  SLOT( editCard() ) );

#if defined(Q_WS_QWS)
#if !defined(QT_NO_COP)
    QCopChannel *todayChannel = new QCopChannel( "QPE/Today" , this );
    connect ( todayChannel, SIGNAL( received( const QCString &, const QByteArray &) ),
              this, SLOT ( channelReceived( const QCString &, const QByteArray &) ) );
#endif
#endif

    setOwnerField();
    m_refreshTimer = new QTimer( this );
    connect( m_refreshTimer, SIGNAL( timeout() ), this, SLOT( refresh() ) );
    m_refreshTimer->start( 15000 );
    refresh();
    showMaximized();
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
    Config cfg( "today" );

    cfg.setGroup( "Plugins" );
    m_excludeApplets = cfg.readListEntry( "ExcludeApplets", ',' );
    m_allApplets = cfg.readListEntry( "AllApplets", ',' );

    cfg.setGroup( "General" );
    m_iconSize = cfg.readNumEntry( "IconSize", 18 );
    setRefreshTimer(  cfg.readNumEntry( "checkinterval", 15000 ) );
}


/**
 * Load the plugins
 */
void Today::loadPlugins() {

    // extra list for plugins that exclude themself from periodic refresh
    QMap<QString, TodayPlugin> pluginListRefreshExclude;

    QValueList<TodayPlugin>::Iterator tit;
    if ( !pluginList.isEmpty() ) {
        for ( tit = pluginList.begin(); tit != pluginList.end(); ++tit ) {
            if ( (*tit).excludeRefresh ) {
                pluginListRefreshExclude.insert( (*tit).name , (*tit) );
                qDebug( "Found an plug that does not want refresh feature" );
            } else {
                (*tit).library->unload();
                delete (*tit).library;
            }
        }
        pluginList.clear();
    }


    QString path = QPEApplication::qpeDir() + "/plugins/today";
    QDir dir( path, "lib*.so" );

    QStringList list = dir.entryList();
    QStringList::Iterator it;

    QMap<QString, TodayPlugin> tempList;

    for ( it = list.begin(); it != list.end(); ++it ) {
	TodayPluginInterface *iface = 0;
	QLibrary *lib = new QLibrary( path + "/" + *it );

	qDebug( "querying: %s", QString( path + "/" + *it ).latin1() );
        if ( lib->queryInterface( IID_TodayPluginInterface, (QUnknownInterface**)&iface ) == QS_OK ) {
	    qDebug( "loading: %s", QString( path + "/" + *it ).latin1() );
            qDebug( QString(*it) );

            // If plugin is exludes from refresh, get it in the list again here.

            if ( pluginListRefreshExclude.contains( (*it) ) ) {
                tempList.insert( pluginListRefreshExclude[(*it)].name, pluginListRefreshExclude[(*it)] );
                qDebug( "TEST2 " +  pluginListRefreshExclude[(*it)].name );
            } else {

                TodayPlugin plugin;
                plugin.library = lib;
                plugin.iface = iface;
                plugin.name = QString(*it);

                // find out if plugins should be shown
                if ( m_excludeApplets.grep( *it ).isEmpty() ) {
                    plugin.active = true;
                } else {
                    plugin.active = false;
                }
                plugin.guiPart = plugin.iface->guiPart();
                plugin.excludeRefresh = plugin.guiPart->excludeFromRefresh();

                // package the whole thing into a qwidget so it can be shown and hidden
                plugin.guiBox = new QWidget( this );
                QHBoxLayout *boxLayout = new QHBoxLayout( plugin.guiBox );
                QPixmap plugPix;
                plugPix.convertFromImage( Resource::loadImage( plugin.guiPart->pixmapNameWidget() ).smoothScale( m_iconSize, m_iconSize ), 0 );
                OClickableLabel* plugIcon = new OClickableLabel( plugin.guiBox );
                plugIcon->setPixmap( plugPix );
                QWhatsThis::add( plugIcon, tr("Click here to launch the associated app") );
                plugIcon->setName( plugin.guiPart->appName() );
                connect( plugIcon, SIGNAL( clicked() ), this, SLOT( startApplication() ) );
                // a scrollview for each plugin
                QScrollView* sv = new QScrollView( plugin.guiBox );
                QWidget *plugWidget = plugin.guiPart->widget( sv->viewport() );
                // not sure if that is good .-)
                sv->setMinimumHeight( 10 );
                sv->setResizePolicy( QScrollView::AutoOneFit );
                sv->setHScrollBarMode( QScrollView::AlwaysOff );
                sv->setFrameShape( QFrame::NoFrame );
                sv->addChild( plugWidget );
                // make sure the icon is on the top alligned
                boxLayout->addWidget( plugIcon, 0, AlignTop );
                boxLayout->addWidget( sv, 0, AlignTop );
                boxLayout->setStretchFactor( plugIcon, 1 );
                boxLayout->setStretchFactor( sv, 9 );
                // "prebuffer" it in one more list, to get the sorting done
                tempList.insert( plugin.name, plugin );

                // on first start the list is off course empty
                if ( m_allApplets.isEmpty() ) {
                    layout->addWidget( plugin.guiBox );
                    pluginList.append( plugin );
                }
            }
        } else {
            qDebug( "could not recognize %s", QString( path + "/" + *it ).latin1() );
            delete lib;
        }
    }

    if ( !m_allApplets.isEmpty() ) {
        TodayPlugin tempPlugin;
        QStringList::Iterator stringit;

        for( stringit = m_allApplets.begin(); stringit !=  m_allApplets.end(); ++stringit ) {
            tempPlugin = ( tempList.find( *stringit ) ).data();
            if ( !( (tempPlugin.name).isEmpty() ) ) {
                layout->addWidget( tempPlugin.guiBox );
                pluginList.append( tempPlugin );
            }
        }
    }
}


/**
 * Repaint method. Reread all fields.
 */
void Today::draw() {

    if ( pluginList.count() == 0 ) {
        QLabel *noPlugins = new QLabel( this );
        noPlugins->setText( tr( "No plugins found" ) );
        layout->addWidget( noPlugins );
        return;
    }

    uint count = 0;
    TodayPlugin plugin;
    for ( uint i = 0; i < pluginList.count(); i++ ) {
	plugin = pluginList[i];

    	if ( plugin.active ) {
            //   qDebug( plugin.name + " is ACTIVE " );
            plugin.guiBox->show();
        } else {
            //  qDebug( plugin.name + " is INACTIVE" );
            plugin.guiBox->hide();
        }
        count++;
    }

    if ( count == 0 ) {
        QLabel *noPluginsActive = new QLabel( this );
        noPluginsActive->setText( tr( "No plugins activated" ) );
        layout->addWidget( noPluginsActive );
    }
    layout->addStretch(0);
}


/**
 * The method for the configuration dialog.
 */
void Today::startConfig() {

    // disconnect timer to prevent problems while being on config dialog
    disconnect( m_refreshTimer, SIGNAL( timeout() ), this, SLOT( refresh() ) );

    TodayConfig conf( this, "dialog", true );

    TodayPlugin plugin;
    QList<TodayConfigWidget> configWidgetList;

    for ( int i = pluginList.count() - 1  ; i >= 0; i-- ) {
        plugin = pluginList[i];

        // load the config widgets in the tabs
        if ( plugin.guiPart->configWidget( this ) != 0l ) {
            TodayConfigWidget* widget = plugin.guiPart->configWidget( conf.TabWidget3  );
            configWidgetList.append( widget );
            conf.TabWidget3->addTab( widget, plugin.guiPart->pixmapNameConfig()
                                     , plugin.guiPart->appName() );
        }
        // set the order/activate tab
        conf.pluginManagement( plugin.name, plugin.guiPart->pluginName(),
                              Resource::loadPixmap( plugin.guiPart->pixmapNameWidget() ) );
    }

    if ( conf.exec() == QDialog::Accepted ) {
        conf.writeConfig();
        TodayConfigWidget *confWidget;
        for ( confWidget = configWidgetList.first(); confWidget != 0;
              confWidget = configWidgetList.next() ) {
            confWidget->writeConfig();
        }
        refresh();
    } else {
        // since refresh is not called in that case , reconnect the signal
        connect( m_refreshTimer, SIGNAL( timeout() ), this, SLOT( refresh() ) );
    }
}


/**
 * Refresh for the view. Reload all applets
 *
 */
void Today::refresh() {
    init();

    qDebug(" refresh ");
    // set the date in top label
    QDate date = QDate::currentDate();
    QString time = ( tr( date.toString() ) );

    DateLabel->setText( QString( "<font color=#FFFFFF>" + time + "</font>" ) );

    if ( layout ) {
        delete layout;
    }
    layout = new QVBoxLayout( this );
    layout->addWidget( Frame );
    layout->addWidget( OwnerField );

    loadPlugins();
    draw();
}

void Today::startAddressbook() {
    QCopEnvelope e( "QPE/System", "execute(QString)" );
    e << QString( "addressbook" );
}


void Today::startApplication() {
    QCopEnvelope e( "QPE/System", "execute(QString)" );
    e << QString( sender()->name() );
}

/**
 * launch addressbook (personal card)
 */
void Today::editCard() {
    startAddressbook();
    while( !QCopChannel::isRegistered( "QPE/Addressbook" ) ) {
        qApp->processEvents();
    }
    QCopEnvelope v( "QPE/Addressbook", "editPersonalAndClose()" );
}

Today::~Today() {
}

