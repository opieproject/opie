/*
 * today.cpp
 *
 * copyright   : (c) 2002,2003 by Maximilian Reiﬂ
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



#define QTOPIA_INTERNAL_LANGLIST

#include "today.h"

#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>
#include <qpe/global.h>
#include <qpe/qpeapplication.h>
#include <qpe/contact.h>
#include <qpe/timestring.h>

#include <qdir.h>
#include <qfile.h>
#include <qtimer.h>
#include <qwhatsthis.h>
#include <qtranslator.h>

struct TodayPlugin {
    TodayPlugin() : library( 0 ), iface( 0 ), guiPart( 0 ), guiBox( 0 ) {}
    QLibrary *library;
    QInterfacePtr<TodayPluginInterface> iface;
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
    big_box = 0L;


    layout = new QVBoxLayout( this );
    layout->addWidget( Frame );
    layout->addWidget( OwnerField );

    sv = new QScrollView( this );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setHScrollBarMode( QScrollView::AlwaysOff );
    sv->setFrameShape( QFrame::NoFrame );

    layout->addWidget( sv );
    layout->setStretchFactor( sv,9 );

    loadPlugins();
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
    m_hideBanner = cfg.readNumEntry( "HideBanner", 0 );
    setRefreshTimer( cfg.readNumEntry( "checkinterval", 15000 ) );

    // set the date in top label
    QDate date = QDate::currentDate();
    DateLabel->setText( QString( "<font color=#FFFFFF>" + TimeString::longDateString( date ) + "</font>" ) );

    if ( m_hideBanner )  {
        Opiezilla->hide();
        TodayLabel->hide();
    } else {
        Opiezilla->show();
        TodayLabel->show();
    }

    if ( big_box ) {
        delete big_box;
    }

    big_box = new QVBox( sv->viewport() );
    sv->addChild( big_box );
}

/**
 * Load the plugins
 */
void Today::loadPlugins() {

    init();

    QValueList<TodayPlugin>::Iterator tit;
    if ( !pluginList.isEmpty() ) {
        for ( tit = pluginList.begin(); tit != pluginList.end(); ++tit ) {
            (*tit).guiBox->hide();
            (*tit).guiBox->reparent( 0, QPoint( 0, 0 ) );
            delete (*tit).guiBox;
            (*tit).library->unload();
            delete (*tit).library;
        }
        pluginList.clear();
    }

    QString path = QPEApplication::qpeDir() + "/plugins/today";
    QDir dir( path, "lib*.so" );

    QStringList list = dir.entryList();
    QStringList::Iterator it;

    QMap<QString, TodayPlugin> tempList;

    for ( it = list.begin(); it != list.end(); ++it ) {
        QInterfacePtr<TodayPluginInterface> iface;
        QLibrary *lib = new QLibrary( path + "/" + *it );

	qDebug( "querying: %s", QString( path + "/" + *it ).latin1() );
        if ( lib->queryInterface( IID_TodayPluginInterface, (QUnknownInterface**)&iface ) == QS_OK ) {
	    qDebug( "accepted: %s", QString( path + "/" + *it ).latin1() );
            qDebug( QString(*it) );

            TodayPlugin plugin;
            plugin.library = lib;
            plugin.iface = iface;
            plugin.name = QString(*it);

            QString type = (*it).left( (*it).find(".") );

            // grr, sharp rom does not know Global::languageList();
            //            QStringList langs = Global::languageList();
            QString tfn = QPEApplication::qpeDir() + "/i18n/";
            QDir langDir = tfn;
            QStringList langs = langDir.entryList("*", QDir::Dirs );

            for (QStringList::ConstIterator lit = langs.begin(); lit!=langs.end(); ++lit) {
                QString lang = *lit;
                qDebug( "Languages: " + lang );
                QTranslator * trans = new QTranslator( qApp );
                QString tfn = QPEApplication::qpeDir()+"/i18n/" + lang + "/" + type + ".qm";
                if ( trans->load( tfn ) )  {
                    qApp->installTranslator( trans );
                }  else {
                    delete trans;
                }
            }

            // find out if plugins should be shown
            if ( m_excludeApplets.grep( *it ).isEmpty() ) {
                plugin.active = true;
            } else {
                plugin.active = false;
            }

            plugin.guiPart = plugin.iface->guiPart();
            plugin.excludeRefresh = plugin.guiPart->excludeFromRefresh();

            // package the whole thing into a qwidget so it can be shown and hidden
            plugin.guiBox = new QWidget( big_box );
            QHBoxLayout *boxLayout = new QHBoxLayout( plugin.guiBox );
            QPixmap plugPix;
            plugPix.convertFromImage( Resource::loadImage( plugin.guiPart->pixmapNameWidget() ).smoothScale( m_iconSize, m_iconSize ), 0 );
            OClickableLabel* plugIcon = new OClickableLabel( plugin.guiBox );
            plugIcon->setPixmap( plugPix );
            QWhatsThis::add( plugIcon, tr("Click here to launch the associated app") );
            plugIcon->setName( plugin.guiPart->appName() );
            connect( plugIcon, SIGNAL( clicked() ), this, SLOT( startApplication() ) );

            QWidget *plugWidget = plugin.guiPart->widget( plugin.guiBox );
            boxLayout->addWidget( plugIcon, 0, AlignTop );
            boxLayout->addWidget( plugWidget, 0, AlignTop );
            boxLayout->setStretchFactor( plugIcon, 1 );
            boxLayout->setStretchFactor( plugWidget, 9 );

           // "prebuffer" it in one more list, to get the sorting done
            tempList.insert( plugin.name, plugin );

            // on first start the list is off course empty
            if ( m_allApplets.isEmpty() ) {
                pluginList.append( plugin );
            }

            // if plugin is not yet in the list, add it to the layout too
            else if ( !m_allApplets.contains( plugin.name ) ) {
                pluginList.append( plugin );
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
                pluginList.append( tempPlugin );
            }
        }
    }
    draw();
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
}


/**
 * The method for the configuration dialog.
 */
void Today::startConfig() {

    // disconnect timer to prevent problems while being on config dialog
    disconnect( m_refreshTimer, SIGNAL( timeout() ), this, SLOT( refresh() ) );
    m_refreshTimer->stop( );

    TodayConfig conf( this, "dialog", true );

    TodayPlugin plugin;
    QList<TodayConfigWidget> configWidgetList;

    for ( int i = pluginList.count() - 1; i >= 0; i-- ) {
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

        // make the plugins to reinitialize ( reread its configs )
        reinitialize();
        draw();

    } else {
        // since refresh is not called in that case , reconnect the signal
        m_refreshTimer->start( 15000 ); // get the config value in here later
        connect( m_refreshTimer, SIGNAL( timeout() ), this, SLOT( refresh() ) );
    }
}



void Today::reinitialize()  {

    Config cfg( "today" );
    cfg.setGroup( "Plugins" );
    m_excludeApplets = cfg.readListEntry( "ExcludeApplets", ',' );


    /* reinitialize all plugins */
    QValueList<TodayPlugin>::Iterator it;
    for ( it = pluginList.begin(); it != pluginList.end(); ++it ) {
        if ( !(*it).excludeRefresh ) {
            (*it).guiPart->reinitialize();
            qDebug( "reinit" );
        }

        /* check if plugins is still to be shown */
        if ( m_excludeApplets.grep( (*it).name ).isEmpty() ) {
            (*it).active = true;
        } else {
            (*it).active = false;
        }

    }
}

/**
 * Refresh for the view. Reload all applets
 *
 */
void Today::refresh() {

    QValueList<TodayPlugin>::Iterator it;
    for ( it = pluginList.begin(); it != pluginList.end(); ++it ) {
        if ( !(*it).excludeRefresh ) {
            (*it).guiPart->refresh();
            qDebug( "refresh" );
        }
    }
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
}

