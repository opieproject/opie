/*
 * todayconfig.cpp
 *
 * copyright   : (c) 2002, 2003, 2004 by Maximilian Reiﬂ
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

#include "todayconfig.h"

#include <opie2/oconfig.h>
#include <opie2/opluginloader.h>
#include <opie2/todayplugininterface.h>

#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>

#include <qcheckbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qheader.h>
#include <qvbox.h>
#include <qtoolbutton.h>
#include <qwhatsthis.h>

using namespace Opie::Ui;
using Opie::Core::OConfig;
using Opie::Core::OPluginManager;
using Opie::Core::OPluginLoader;
using Opie::Core::OPluginItem;

class ToolButton : public QToolButton {

public:
    ToolButton( QWidget *parent, const char *name, const QString& icon, QObject *handler, const QString& slot, bool t = FALSE )
: QToolButton( parent, name ) {
        setPixmap( Resource::loadPixmap( icon ) );
        setAutoRaise( TRUE );
        setFocusPolicy( QWidget::NoFocus );
        setToggleButton( t );
        connect( this, t ? SIGNAL( toggled(bool) ) : SIGNAL( clicked() ), handler, slot );
    }
};


/**
 * The class has currently quite some duplicate code.
 * By that way it would be real easy to have it as seperate app in settings tab
 *
 */
TodayConfig::TodayConfig( QWidget* parent, const char* name, bool modal )
: QDialog( parent, name, modal, WStyle_ContextHelp ) {

    setCaption( tr( "Today Config" ) );

    QVBoxLayout *layout = new QVBoxLayout( this );
    TabWidget3 = new OTabWidget ( this, "tabwidget", OTabWidget::Global, OTabWidget::Bottom );
    layout->addWidget( TabWidget3 );

    tab_2 = new QWidget( TabWidget3, "tab_2" );
    QVBoxLayout *tab2Layout = new QVBoxLayout( tab_2, 4 ,4 );
    QLabel *l = new QLabel( tr( "Load which plugins in what order:" ), tab_2 );
    tab2Layout->addWidget( l );
    QHBox *hbox1 = new QHBox( tab_2 );
    m_appletListView = new QListView( hbox1 );
    m_appletListView->addColumn( "PluginList" );
    m_appletListView->header()->hide();
    m_appletListView->setSorting( -1 );
    QWhatsThis::add
        ( m_appletListView, tr( "Check a checkbox to activate/deactivate a plugin or use the arrow buttons on the right to change the appearance order" ) );
    QVBox *vbox1 = new QVBox( hbox1 );
    new ToolButton( vbox1, tr( "Move Up" ), "up",  this , SLOT( moveSelectedUp() ) );
    new ToolButton( vbox1, tr( "Move Down" ), "down", this , SLOT( moveSelectedDown() ) );
    tab2Layout->addWidget( hbox1 );
    TabWidget3->addTab( tab_2, "pass", tr( "active/order" ) );

    // Misc tab
    tab_3 = new QWidget( TabWidget3, "tab_3" );
    QVBoxLayout *tab3Layout = new QVBoxLayout( tab_3 );

    m_guiMisc = new TodayConfigMiscBase( tab_3 );

    tab3Layout->addWidget( m_guiMisc );
    TabWidget3->addTab( tab_3, "SettingsIcon", tr( "Misc" ) );

    connect ( m_appletListView , SIGNAL( clicked(QListViewItem*) ), this, SLOT( appletChanged() ) );
    previousItem = 0l;
    readConfig();
    QPEApplication::showDialog( this );
}


void TodayConfig::setUpPlugins( OPluginManager * plugManager, OPluginLoader *plugLoader ) {


    m_pluginManager = plugManager;
    m_pluginLoader = plugLoader;

    OPluginItem::List inLst = m_pluginLoader->allAvailable( true );

    OPluginItem::List lst;
    for ( OPluginItem::List::Iterator it = inLst.begin(); it != inLst.end(); ++it ) {
    	lst.prepend((*it));

        TodayPluginInterface* iface = m_pluginLoader->load<TodayPluginInterface>( *it, IID_TodayPluginInterface );

        if ( iface->guiPart()->configWidget(this) != 0l ) {
            TodayConfigWidget* widget = iface->guiPart()->configWidget( TabWidget3  );
            TabWidget3->addTab( widget, iface->guiPart()->pixmapNameConfig()
                                , iface->guiPart()->appName() );
        }
    }

    for ( OPluginItem::List::Iterator it = lst.begin(); it != lst.end(); ++it ) {
        pluginManagement( (*it) );
    }

    TabWidget3->setCurrentTab( tab_2 );
}

/**
 * Autostart, uses the new (opie only) autostart method in the launcher code.
 * If registered against that today ist started on each resume.
 */
void TodayConfig::setAutoStart() {
    OConfig cfg( "today" );
    cfg.setGroup( "Autostart" );
    if ( m_autoStart ) {
        QCopEnvelope e( "QPE/System", "autoStart(QString,QString,QString)" );
        e << QString( "add" );
        e << QString( "today" );
        e << QString( "%1" ).arg( m_autoStartTimer );
    } else {
        QCopEnvelope e( "QPE/System", "autoStart(QString,QString)" );
        e << QString( "remove" );
        e << QString( "today" );
    }
}

/**
 * Read the config part
 */
void TodayConfig::readConfig() {
    OConfig cfg( "today" );
    cfg.setGroup( "Autostart" );
    m_autoStart = cfg.readNumEntry( "autostart", 1 );
    m_guiMisc->CheckBoxAuto->setChecked( m_autoStart );
    m_autoStartTimer = cfg.readNumEntry( "autostartdelay", 0 );
    m_guiMisc->SpinBoxTime->setValue( m_autoStartTimer );

    cfg.setGroup( "General" );
    m_iconSize = cfg.readNumEntry( "IconSize", 18 );
    m_guiMisc->SpinBoxIconSize->setValue( m_iconSize );
    m_guiMisc->SpinRefresh->setValue( cfg.readNumEntry( "checkinterval", 15000 ) / 1000 );
    m_guiMisc->CheckBoxHide->setChecked( cfg.readNumEntry( "HideBanner", 0 ) );
}

/**
 * Write the config part
 */
void TodayConfig::writeConfig() {
    OConfig cfg( "today" );

    int position = m_appletListView->childCount();

    QListViewItemIterator list_it( m_appletListView );
    //
    // this makes sure the names get saved in the order selected
    for ( ; list_it.current(); ++list_it ) {
        OPluginItem::List lst = m_pluginLoader->allAvailable( true );
        for ( OPluginItem::List::Iterator it = lst.begin(); it != lst.end(); ++it ) {
            if ( QString::compare( (*it).name() , list_it.current()->text(0) ) == 0 ) {
                (*it).setPosition(position--);
                m_pluginManager->setEnabled( (*it),((QCheckListItem*)list_it.current())->isOn() );
            }
        }
    }

    m_pluginManager->save();

    cfg.setGroup( "Autostart" );
    m_autoStart = m_guiMisc->CheckBoxAuto->isChecked();
    cfg.writeEntry( "autostart", m_autoStart );
    m_autoStartTimer = m_guiMisc->SpinBoxTime->value();
    cfg.writeEntry( "autostartdelay", m_autoStartTimer );
    m_iconSize = m_guiMisc->SpinBoxIconSize->value();

    cfg.setGroup( "General" );
    cfg.writeEntry( "IconSize", m_iconSize );
    cfg.writeEntry( "HideBanner",  m_guiMisc->CheckBoxHide->isChecked() );
    cfg.writeEntry( "checkinterval", m_guiMisc->SpinRefresh->value()*1000 );

    // set autostart settings
    setAutoStart();

    OPluginItem::List lst = m_pluginManager->managedPlugins();
    for ( OPluginItem::List::Iterator it = lst.begin(); it != lst.end(); ++it ) {
        TodayPluginInterface* iface = m_pluginLoader->load<TodayPluginInterface>( *it, IID_TodayPluginInterface );
        if ( iface->guiPart()->configWidget(this) != 0l ) {
            iface->guiPart()->configWidget(this)->writeConfig();
        }
    }
}


void TodayConfig::moveSelectedUp() {
    QListViewItem *item = m_appletListView->selectedItem();
    if ( item && item->itemAbove() ) {
        item->itemAbove()->moveItem( item );
    }
}


void TodayConfig::moveSelectedDown() {
    QListViewItem *item = m_appletListView->selectedItem();
    if ( item && item->itemBelow() ) {
        item->moveItem( item->itemBelow() );
    }
}


/**
 * Set up the icons in the order/active tab
 */
void TodayConfig::pluginManagement( OPluginItem plugItem ) {

    QCheckListItem *item = new QCheckListItem( m_appletListView, plugItem.name(), QCheckListItem::CheckBox );

    TodayPluginInterface* iface = m_pluginLoader->load<TodayPluginInterface>( plugItem, IID_TodayPluginInterface );
    QPixmap icon = Resource::loadPixmap( iface->guiPart()->pixmapNameWidget() );
    if ( !icon.isNull() ) {
        item->setPixmap( 0, icon );
    }
    item->setOn( plugItem.isEnabled() );
    previousItem = item;
}



TodayConfig::~TodayConfig() {}
