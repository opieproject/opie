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
#include "configwidget.h"

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


struct TodayPlugin {
    QLibrary *library;
    TodayPluginInterface *iface;
    TodayPluginObject *guiPart;
    QWidget *guiBox;
    QString name;
    bool active;
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
}


/**
 * Load the plugins
 */
void Today::loadPlugins() {

    QValueList<TodayPlugin>::Iterator tit;
    for ( tit = pluginList.begin(); tit != pluginList.end(); ++tit ) {
	(*tit).library->unload();
	delete (*tit).library;
    }
    pluginList.clear();

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

            // package the whole thing into a qwidget so it can be shown and hidden
            plugin.guiBox = new QWidget( this );
            QHBoxLayout *boxLayout = new QHBoxLayout( plugin.guiBox );
            QPixmap plugPix;
            plugPix.convertFromImage( Resource::loadImage( plugin.guiPart->pixmapNameWidget() ).smoothScale( 18, 18 ), 0 );
            OClickableLabel* plugIcon = new OClickableLabel( plugin.guiBox );
            plugIcon->setPixmap( plugPix );
            // a scrollview for each plugin
            QScrollView* sv = new QScrollView( plugin.guiBox );
            QWidget *plugWidget = plugin.guiPart->widget( sv->viewport() );
            sv->setMinimumHeight( plugin.guiPart->minHeight() );
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

    TodayConfig conf( this, "dialog", true );

    TodayPlugin plugin;
    QList<ConfigWidget> configWidgetList;

    for ( int i = pluginList.count() - 1  ; i >= 0; i-- ) {
        plugin = pluginList[i];

        // load the config widgets in the tabs
        if ( plugin.guiPart->configWidget( this ) != 0l ) {
            ConfigWidget* widget = plugin.guiPart->configWidget( conf.TabWidget3  );
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
        ConfigWidget *confWidget;
        for ( confWidget = configWidgetList.first(); confWidget != 0;
              confWidget = configWidgetList.next() ) {
            confWidget->writeConfig();
        }
        refresh();
    }
}


/**
 * Refresh for the view. Reload all applets
 *
 */
void Today::refresh() {
    init();

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

