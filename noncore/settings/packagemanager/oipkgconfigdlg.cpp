/*
                             This file is part of the Opie Project

                             Copyright (C)2004, 2005 Dan Williams <drw@handhelds.org>
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

#include "oipkgconfigdlg.h"

#include <opie2/ofiledialog.h>
#include <opie2/oresource.h>

#include <qpe/qpeapplication.h>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qscrollview.h>
#include <qwhatsthis.h>

OIpkgConfigDlg::OIpkgConfigDlg( OIpkg *ipkg, bool installOptions, QWidget *parent )
    : QDialog( parent, QString::null, true, WStyle_ContextHelp )
    , m_ipkg( ipkg )
    , m_configs( 0l )
    , m_installOptions( installOptions )
    , m_serverCurrent( -1 )
    , m_destCurrent( -1 )
    , m_layout( this, 2, 4 )
    , m_tabWidget( this )
{
    setCaption( tr( "Configuration" ) );

    // Initialize configuration widgets
    if ( !installOptions )
    {
        initServerWidget();
        initDestinationWidget();
        initProxyWidget();
    }
    initOptionsWidget();

    // Load configuration information
    initData();

    // Setup tabs for all info
    m_layout.addWidget( &m_tabWidget );
    if ( !m_installOptions )
    {
        m_tabWidget.addTab( m_serverWidget, "packagemanager/servertab", tr( "Servers" ) );
        m_tabWidget.addTab( m_destWidget, "packagemanager/desttab", tr( "Destinations" ) );
        m_tabWidget.addTab( m_proxyWidget, "packagemanager/proxytab", tr( "Proxies" ) );
        m_tabWidget.addTab( m_optionsWidget, "exec", tr( "Options" ) );
        m_tabWidget.setCurrentTab( tr( "Servers" ) );
    }
    else
    {
        m_tabWidget.addTab( m_optionsWidget, "exec", tr( "Options" ) );
    }
}

void OIpkgConfigDlg::accept()
{
    // Save server, destination and proxy configuration
    if ( !m_installOptions )
    {
        // Update proxy information before saving settings
        OConfItem *confItem = m_ipkg->findConfItem( OConfItem::Option, "http_proxy" );
        if ( confItem )
        {
            confItem->setValue( m_proxyHttpServer->text() );
            confItem->setActive( m_proxyHttpActive->isChecked() );
        }
        else
            m_configs->append( new OConfItem( OConfItem::Option, "http_proxy",
                               m_proxyHttpServer->text(), QString::null,
                               m_proxyHttpActive->isChecked() ) );

        confItem = m_ipkg->findConfItem( OConfItem::Option, "ftp_proxy" );
        if ( confItem )
        {
            confItem->setValue( m_proxyFtpServer->text() );
            confItem->setActive( m_proxyFtpActive->isChecked() );
        }
        else
            m_configs->append( new OConfItem( OConfItem::Option, "ftp_proxy",
                               m_proxyFtpServer->text(), QString::null,
                               m_proxyFtpActive->isChecked() ) );

        confItem = m_ipkg->findConfItem( OConfItem::Option, "proxy_username" );
        if ( confItem )
            confItem->setValue( m_proxyUsername->text() );
        else
            m_configs->append( new OConfItem( OConfItem::Option, "proxy_username",
                               m_proxyUsername->text() ) );

        confItem = m_ipkg->findConfItem( OConfItem::Option, "proxy_password" );
        if ( confItem )
            confItem->setValue( m_proxyPassword->text() );
        else
            m_configs->append( new OConfItem( OConfItem::Option, "proxy_password",
                               m_proxyPassword->text() ) );

        QString listsDir = m_optSourceLists->text();
        if ( listsDir == QString::null || listsDir == "" )
            listsDir = "/usr/lib/ipkg/lists"; // TODO - use proper libipkg define
        confItem = m_ipkg->findConfItem( OConfItem::Other, "lists_dir" );
        if ( confItem )
            confItem->setValue( listsDir );
        else
            m_configs->append( new OConfItem( OConfItem::Other, "lists_dir",
                               listsDir, "name" ) );

        m_ipkg->setConfigItems( m_configs );
    }

    // Save options configuration
    int options = 0;
    if ( m_optForceDepends->isChecked() )
        options |= FORCE_DEPENDS;
    if ( m_optForceReinstall->isChecked() )
        options |= FORCE_REINSTALL;
    if ( m_optForceRemove->isChecked() )
        options |= FORCE_REMOVE;
    if ( m_optForceOverwrite->isChecked() )
        options |= FORCE_OVERWRITE;
    if ( m_optForceRecursive->isChecked() )
        options |= FORCE_RECURSIVE;
    if ( m_optVerboseWget->isChecked() )
        options |= FORCE_VERBOSE_WGET;
    m_ipkg->setIpkgExecOptions( options );
    m_ipkg->setIpkgExecVerbosity( m_optVerboseIpkg->currentItem() );

    QDialog::accept();
}

void OIpkgConfigDlg::reject()
{
    if ( m_configs )
        delete m_configs;
}

void OIpkgConfigDlg::initServerWidget()
{
    m_serverWidget = new QWidget( this );

    // Initialize UI
    QVBoxLayout *vb = new QVBoxLayout( m_serverWidget );
    QScrollView *sv = new QScrollView( m_serverWidget );
    vb->addWidget( sv, 0, 0 );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setFrameStyle( QFrame::NoFrame );
    QWidget *container = new QWidget( sv->viewport() );
    sv->addChild( container );
    QGridLayout *layout = new QGridLayout( container, 2, 3, 2, 4 );

    m_serverList = new QListBox( container );
    QWhatsThis::add( m_serverList, tr( "This is a list of all servers configured.  Select one here to edit or delete, or add a new one below." ) );
    m_serverList->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    connect( m_serverList, SIGNAL(highlighted(int)), this, SLOT(slotServerSelected(int)) );
    layout->addMultiCellWidget( m_serverList, 0, 0, 0, 2 );

    QPushButton *btn = new QPushButton( Opie::Core::OResource::loadPixmap( "new", Opie::Core::OResource::SmallIcon ),
                                        tr( "New" ), container );
    btn->setMinimumHeight( AppLnk::smallIconSize()+4 );
    QWhatsThis::add( btn, tr( "Tap here to create a new entry.  Fill in the fields below and then tap on Update." ) );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotServerNew()) );
    layout->addWidget( btn, 1, 0 );

    m_serverEditBtn = new QPushButton( Opie::Core::OResource::loadPixmap( "edit", Opie::Core::OResource::SmallIcon ),
                                       tr( "Edit" ), container );
    m_serverEditBtn->setMinimumHeight( AppLnk::smallIconSize()+4 );
    m_serverEditBtn->setEnabled( false );
    QWhatsThis::add( m_serverEditBtn, tr( "Tap here to edit the entry selected above." ) );
    connect( m_serverEditBtn, SIGNAL(clicked()), this, SLOT(slotServerEdit()) );
    layout->addWidget( m_serverEditBtn, 1, 1 );

    m_serverDeleteBtn = new QPushButton( Opie::Core::OResource::loadPixmap( "trash", Opie::Core::OResource::SmallIcon ),
                                         tr( "Delete" ), container );
    m_serverDeleteBtn->setMinimumHeight( AppLnk::smallIconSize()+4 );
    m_serverDeleteBtn->setEnabled( false );
    QWhatsThis::add( m_serverDeleteBtn, tr( "Tap here to delete the entry selected above." ) );
    connect( m_serverDeleteBtn, SIGNAL(clicked()), this, SLOT(slotServerDelete()) );
    layout->addWidget( m_serverDeleteBtn, 1, 2 );
}

void OIpkgConfigDlg::initDestinationWidget()
{
    m_destWidget = new QWidget( this );

    // Initialize UI
    QVBoxLayout *vb = new QVBoxLayout( m_destWidget );
    QScrollView *sv = new QScrollView( m_destWidget );
    vb->addWidget( sv, 0, 0 );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setFrameStyle( QFrame::NoFrame );
    QWidget *container = new QWidget( sv->viewport() );
    sv->addChild( container );
    QGridLayout *layout = new QGridLayout( container, 2, 3, 2, 4 );

    m_destList = new QListBox( container );
    QWhatsThis::add( m_destList, tr( "This is a list of all destinations configured for this device.  Select one here to edit or delete, or add a new one below." ) );
    m_destList->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    connect( m_destList, SIGNAL(highlighted(int)), this, SLOT(slotDestSelected(int)) );
    layout->addMultiCellWidget( m_destList, 0, 0, 0, 2 );

    QPushButton *btn = new QPushButton( Opie::Core::OResource::loadPixmap( "new", Opie::Core::OResource::SmallIcon ),
                                        tr( "New" ), container );
    btn->setMinimumHeight( AppLnk::smallIconSize()+4 );
    QWhatsThis::add( btn, tr( "Tap here to create a new entry.  Fill in the fields below and then tap on Update." ) );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotDestNew()) );
    layout->addWidget( btn, 1, 0 );

    m_destEditBtn = new QPushButton( Opie::Core::OResource::loadPixmap( "edit", Opie::Core::OResource::SmallIcon ),
                                     tr( "Edit" ), container );
    m_destEditBtn->setMinimumHeight( AppLnk::smallIconSize()+4 );
    m_destEditBtn->setEnabled( false );
    QWhatsThis::add( m_destEditBtn, tr( "Tap here to edit the entry selected above." ) );
    connect( m_destEditBtn, SIGNAL(clicked()), this, SLOT(slotDestEdit()) );
    layout->addWidget( m_destEditBtn, 1, 1 );

    m_destDeleteBtn = new QPushButton( Opie::Core::OResource::loadPixmap( "trash", Opie::Core::OResource::SmallIcon ),
                                       tr( "Delete" ), container );
    m_destDeleteBtn->setMinimumHeight( AppLnk::smallIconSize()+4 );
    m_destDeleteBtn->setEnabled( false );
    QWhatsThis::add( m_destDeleteBtn, tr( "Tap here to delete the entry selected above." ) );
    connect( m_destDeleteBtn, SIGNAL(clicked()), this, SLOT(slotDestDelete()) );
    layout->addWidget( m_destDeleteBtn, 1, 2 );
}

void OIpkgConfigDlg::initProxyWidget()
{
    m_proxyWidget = new QWidget( this );

    // Initialize UI
    QVBoxLayout *vb = new QVBoxLayout( m_proxyWidget );
    QScrollView *sv = new QScrollView( m_proxyWidget );
    vb->addWidget( sv, 0, 0 );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setFrameStyle( QFrame::NoFrame );
    QWidget *container = new QWidget( sv->viewport() );
    sv->addChild( container );
    QGridLayout *layout = new QGridLayout( container, 4, 2, 2, 4 );

    // HTTP proxy server configuration
    QGroupBox *grpbox = new QGroupBox( 0, Qt::Vertical, tr( "HTTP Proxy" ), container );
    grpbox->layout()->setSpacing( 2 );
    grpbox->layout()->setMargin( 4 );
    layout->addMultiCellWidget( grpbox, 0, 0, 0, 1 );
    QVBoxLayout *grplayout = new QVBoxLayout( grpbox->layout() );
    m_proxyHttpServer = new QLineEdit( grpbox );
    QWhatsThis::add( m_proxyHttpServer, tr( "Enter the URL address of the HTTP proxy server here." ) );
    grplayout->addWidget( m_proxyHttpServer );
    m_proxyHttpActive = new QCheckBox( tr( "Enabled" ), grpbox );
    QWhatsThis::add( m_proxyHttpActive, tr( "Tap here to enable or disable the HTTP proxy server." ) );
    grplayout->addWidget( m_proxyHttpActive );

    // FTP proxy server configuration
    grpbox = new QGroupBox( 0, Qt::Vertical, tr( "FTP Proxy" ), container );
    grpbox->layout()->setSpacing( 2 );
    grpbox->layout()->setMargin( 4 );
    layout->addMultiCellWidget( grpbox, 1, 1, 0, 1 );
    grplayout = new QVBoxLayout( grpbox->layout() );
    m_proxyFtpServer = new QLineEdit( grpbox );
    QWhatsThis::add( m_proxyFtpServer, tr( "Enter the URL address of the FTP proxy server here." ) );
    grplayout->addWidget( m_proxyFtpServer );
    m_proxyFtpActive = new QCheckBox( tr( "Enabled" ), grpbox );
    QWhatsThis::add( m_proxyFtpActive, tr( "Tap here to enable or disable the FTP proxy server." ) );
    grplayout->addWidget( m_proxyFtpActive );

    // Proxy server username and password configuration
    QLabel *label = new QLabel( tr( "Username:" ), container );
    QWhatsThis::add( label, tr( "Enter the username for the proxy servers here." ) );
    layout->addWidget( label, 2, 0 );
    m_proxyUsername = new QLineEdit( container );
    QWhatsThis::add( m_proxyUsername, tr( "Enter the username for the proxy servers here." ) );
    layout->addWidget( m_proxyUsername, 2, 1 );

    label = new QLabel( tr( "Password:" ), container );
    QWhatsThis::add( label, tr( "Enter the password for the proxy servers here." ) );
    layout->addWidget( label, 3, 0 );
    m_proxyPassword = new QLineEdit( container );
    QWhatsThis::add( m_proxyPassword, tr( "Enter the password for the proxy servers here." ) );
    layout->addWidget( m_proxyPassword, 3, 1 );
}

void OIpkgConfigDlg::initOptionsWidget()
{
    m_optionsWidget = new QWidget( this );

    // Initialize UI
    QVBoxLayout *vb = new QVBoxLayout( m_optionsWidget );
    QScrollView *sv = new QScrollView( m_optionsWidget );
    vb->addWidget( sv, 0, 0 );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setFrameStyle( QFrame::NoFrame );
    QWidget *container = new QWidget( sv->viewport() );
    sv->addChild( container );
    QGridLayout *layout = new QGridLayout( container, 8, 2, 2, 4 );

    m_optForceDepends = new QCheckBox( tr( "Force Depends" ), container );
    QWhatsThis::add( m_optForceDepends, tr( "Tap here to enable or disable the '-force-depends' option for Ipkg." ) );
    layout->addMultiCellWidget( m_optForceDepends, 0, 0, 0, 1 );

    m_optForceReinstall = new QCheckBox( tr( "Force Reinstall" ), container );
    QWhatsThis::add( m_optForceReinstall, tr( "Tap here to enable or disable the '-force-reinstall' option for Ipkg." ) );
    layout->addMultiCellWidget( m_optForceReinstall, 1, 1, 0, 1 );

    m_optForceRemove = new QCheckBox( tr( "Force Remove" ), container );
    QWhatsThis::add( m_optForceRemove, tr( "Tap here to enable or disable the '-force-removal-of-dependent-packages' option for Ipkg." ) );
    layout->addMultiCellWidget( m_optForceRemove, 2, 2, 0, 1 );

    m_optForceOverwrite = new QCheckBox( tr( "Force Overwrite" ), container );
    QWhatsThis::add( m_optForceOverwrite, tr( "Tap here to enable or disable the '-force-overwrite' option for Ipkg." ) );
    layout->addMultiCellWidget( m_optForceOverwrite, 3, 3, 0, 1 );

    m_optForceRecursive = new QCheckBox( tr( "Force Recursive" ), container );
    QWhatsThis::add( m_optForceRecursive, tr( "Tap here to enable or disable the '-recursive' option for Ipkg." ) );
    layout->addMultiCellWidget( m_optForceRecursive, 4, 4, 0, 1 );

    m_optVerboseWget = new QCheckBox( tr( "Verbose fetch" ), container );
    QWhatsThis::add( m_optVerboseWget, tr( "Tap here to enable or disable the '-verbose_wget' option for Ipkg." ) );
    layout->addMultiCellWidget( m_optVerboseWget, 5, 5, 0, 1 );

    QLabel *l = new QLabel( tr( "Information level:" ), container );
    QWhatsThis::add( l, tr( "Select information level for Ipkg." ) );
    layout->addMultiCellWidget( l, 6, 6, 0, 1 );

    m_optVerboseIpkg = new QComboBox( container );
    QWhatsThis::add( m_optVerboseIpkg, tr( "Select information level for Ipkg." ) );
    m_optVerboseIpkg->insertItem( tr( "Errors only" ) );
    m_optVerboseIpkg->insertItem( tr( "Normal messages" ) );
    m_optVerboseIpkg->insertItem( tr( "Informative messages" ) );
    m_optVerboseIpkg->insertItem( tr( "Troubleshooting output" ) );
    layout->addMultiCellWidget( m_optVerboseIpkg, 7, 7, 0, 1 );

    l = new QLabel( tr( "Package source lists directory:" ), container );
    QWhatsThis::add( l, tr( "Enter the directory where package source feed information is stored." ) );
    layout->addMultiCellWidget( l, 8, 8, 0, 1 );

    m_optSourceLists = new QLineEdit( container );
    QWhatsThis::add( m_optSourceLists, tr( "Enter the directory where package source feed information is stored." ) );
    layout->addWidget( m_optSourceLists, 9, 0 );

    QPushButton *btn = new QPushButton( Opie::Core::OResource::loadPixmap( "folder", Opie::Core::OResource::SmallIcon ),
                                        QString::null, container );
    btn->setMinimumHeight( AppLnk::smallIconSize()+4 );
    btn->setMaximumWidth( btn->height() );
    QWhatsThis::add( btn, tr( "Tap here to select the directory where package source feed information is stored." ) );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotOptSelectSourceListsPath()) );
    layout->addWidget( btn, 9, 1 );

    layout->addItem( new QSpacerItem( 1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
}

void OIpkgConfigDlg::initData()
{
    // Read ipkg configuration (server/destination/proxy) information
    if ( m_ipkg && !m_installOptions )
    {
        m_configs = m_ipkg->configItems();
        if ( m_configs )
        {
            for ( OConfItemListIterator configIt( *m_configs ); configIt.current(); ++configIt )
            {
                OConfItem *config = configIt.current();

                // Add configuration item to the appropriate dialog controls
                if ( config )
                {
                    switch ( config->type() )
                    {
                        case OConfItem::Source :  m_serverList->insertItem( config->name() ); break;
                        case OConfItem::Destination : m_destList->insertItem( config->name() ); break;
                        case OConfItem::Option :
                            {
                                if ( config->name() == "http_proxy" )
                                {
                                    m_proxyHttpServer->setText( config->value() );
                                    m_proxyHttpActive->setChecked( config->active() );
                                }
                                else if ( config->name() == "ftp_proxy" )
                                {
                                    m_proxyFtpServer->setText( config->value() );
                                    m_proxyFtpActive->setChecked( config->active() );
                                }
                                else if ( config->name() == "proxy_username" )
                                {
                                    m_proxyUsername->setText( config->value() );
                                }
                                else if ( config->name() == "proxy_password" )
                                {
                                    m_proxyPassword->setText( config->value() );
                                }
                            }
                            break;
                        case OConfItem::Other :
                            {
                                if ( config->name() == "lists_dir" )
                                    m_optSourceLists->setText( config->value() );
                                else // TODO - use proper libipkg define
                                    m_optSourceLists->setText( "/usr/lib/ipkg/lists" );
                            }
                            break;
                        default : break;
                    };
                }
            }
        }
    }

    // Get Ipkg execution options
    int options = m_ipkg->ipkgExecOptions();
    if ( options & FORCE_DEPENDS )
        m_optForceDepends->setChecked( true );
    if ( options & FORCE_REINSTALL )
        m_optForceReinstall->setChecked( true );
    if ( options & FORCE_REMOVE )
        m_optForceRemove->setChecked( true );
    if ( options & FORCE_OVERWRITE )
        m_optForceOverwrite->setChecked( true );
    if ( options & FORCE_RECURSIVE )
        m_optForceRecursive->setChecked( true );
    if ( options & FORCE_VERBOSE_WGET )
        m_optVerboseWget->setChecked( true );

    m_optVerboseIpkg->setCurrentItem( m_ipkg->ipkgExecVerbosity() );
}

void OIpkgConfigDlg::slotServerSelected( int index )
{
    m_serverCurrent = index;

    // Enable Edit and Delete buttons
    m_serverEditBtn->setEnabled( true );
    m_serverDeleteBtn->setEnabled( true );
}

void OIpkgConfigDlg::slotServerNew()
{
    OConfItem *server = new OConfItem( OConfItem::Source );

    OIpkgServerDlg dlg( server, this );
    if ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted )
    {
        // Add to configuration option list
        m_configs->append( server );
        m_configs->sort();

        // Add to server list
        m_serverList->insertItem( server->name() );
        m_serverList->setCurrentItem( m_serverList->count() );
    }
    else
        delete server;
}

void OIpkgConfigDlg::slotServerEdit()
{
    // Find selected server in list
    OConfItem *server = m_ipkg->findConfItem( OConfItem::Source, m_serverList->currentText() );

    // Edit server
    if ( server )
    {
        QString origName = server->name();
        OIpkgServerDlg dlg( server, this );
        if ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted )
        {
            // Check to see if name has changed, if so update the server list
            if ( server->name() != origName )
                m_serverList->changeItem( server->name(), m_serverCurrent );
        }
    }
}

void OIpkgConfigDlg::slotServerDelete()
{
    // Find selected server in list
    OConfItem *server = m_ipkg->findConfItem( OConfItem::Source, m_serverList->currentText() );

    // Delete server
    if ( server )
    {
        m_configs->removeRef( server );
        m_serverList->removeItem( m_serverCurrent );
    }
}

void OIpkgConfigDlg::slotDestSelected( int index )
{
    m_destCurrent = index;

    // Enable Edit and Delete buttons
    m_destEditBtn->setEnabled( true );
    m_destDeleteBtn->setEnabled( true );
}

void OIpkgConfigDlg::slotDestNew()
{
    OConfItem *dest = new OConfItem( OConfItem::Destination );

    OIpkgDestDlg dlg( dest, this );
    if ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted )
    {
        // Add to configuration option list
        m_configs->append( dest );
        m_configs->sort();

        // Add to destination list
        m_destList->insertItem( dest->name() );
        m_destList->setCurrentItem( m_destList->count() );
    }
    else
        delete dest;
}

void OIpkgConfigDlg::slotDestEdit()
{
    // Find selected destination in list
    OConfItem *dest = m_ipkg->findConfItem( OConfItem::Destination, m_destList->currentText() );

    // Edit destination
    if ( dest )
    {
        QString origName = dest->name();
        OIpkgDestDlg dlg( dest, this );
        if ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted )
        {
            // Check to see if name has changed, if so update the dest list
            if ( dest->name() != origName )
                m_destList->changeItem( dest->name(), m_destCurrent );
        }
    }
}

void OIpkgConfigDlg::slotDestDelete()
{
    // Find selected destination in list
    OConfItem *destination = m_ipkg->findConfItem( OConfItem::Destination, m_destList->currentText() );

    // Delete destination
    if ( destination )
    {
        m_configs->removeRef( destination );
        m_destList->removeItem( m_destCurrent );
    }
}

void OIpkgConfigDlg::slotOptSelectSourceListsPath()
{
    QString path = Opie::Ui::OFileDialog::getDirectory( 0, m_optSourceLists->text() );
    if ( path.at( path.length() - 1 ) == '/' )
        path.truncate( path.length() - 1 );
    if ( !path.isNull() )
        m_optSourceLists->setText( path );
}

OIpkgServerDlg::OIpkgServerDlg( OConfItem *server, QWidget *parent )
    : QDialog( parent, QString::null, true, WStyle_ContextHelp )
    , m_server( server )
{
    setCaption( tr( "Edit Server" ) );

    // Initialize UI
    QVBoxLayout *layout = new QVBoxLayout( this, 2, 4 );

    m_active = new QCheckBox( tr( "Active" ), this );
    QWhatsThis::add( m_active, tr( "Tap here to indicate whether this entry is active or not." ) );
    layout->addWidget( m_active );

    layout->addStretch();

    QLabel *label = new QLabel( tr( "Name:" ), this );
    QWhatsThis::add( label, tr( "Enter the name of this entry here." ) );
    layout->addWidget( label );
    m_name = new QLineEdit( this );
    QWhatsThis::add( m_name, tr( "Enter the name of this entry here." ) );
    layout->addWidget( m_name );

    layout->addStretch();

    label = new QLabel( tr( "Address:" ), this );
    QWhatsThis::add( label, tr( "Enter the URL address of this entry here." ) );
    layout->addWidget( label );
    m_location = new QLineEdit( this );
    QWhatsThis::add( m_location, tr( "Enter the URL address of this entry here." ) );
    layout->addWidget( m_location );

    layout->addStretch();

    m_compressed = new QCheckBox( tr( "Compressed server feed" ), this );
    QWhatsThis::add( m_compressed, tr( "Tap here to indicate whether the server support compressed archives or not." ) );
    layout->addWidget( m_compressed );

    // Populate initial information
    if ( m_server )
    {
        m_name->setText( m_server->name() );
        m_location->setText( m_server->value() );
        m_compressed->setChecked( m_server->features().contains( "Compressed" ) );
        m_active->setChecked( m_server->active() );
    }
}

void OIpkgServerDlg::accept()
{
    // Save information entered
    QString name = m_name->text();
    name.replace( QRegExp( " " ), "_" );
    m_server->setName( name );
    m_server->setValue( m_location->text() );
    m_compressed->isChecked() ? m_server->setFeatures( "Compressed" )
                              : m_server->setFeatures( QString::null );
    m_server->setActive( m_active->isChecked() );

    QDialog::accept();
}

OIpkgDestDlg::OIpkgDestDlg( OConfItem *dest, QWidget *parent )
    : QDialog( parent, QString::null, true, WStyle_ContextHelp )
    , m_dest( dest )
{
    setCaption( tr( "Edit Destination" ) );

    // Initialize UI
    QVBoxLayout *layout = new QVBoxLayout( this, 2, 4 );

    m_active = new QCheckBox( tr( "Active" ), this );
    QWhatsThis::add( m_active, tr( "Tap here to indicate whether this entry is active or not." ) );
    layout->addWidget( m_active );

    layout->addStretch();

    QLabel *label = new QLabel( tr( "Name:" ), this );
    QWhatsThis::add( label, tr( "Enter the name of this entry here." ) );
    layout->addWidget( label );
    m_name = new QLineEdit( this );
    QWhatsThis::add( m_name, tr( "Enter the name of this entry here." ) );
    layout->addWidget( m_name );

    layout->addStretch();

    label = new QLabel( tr( "Location:" ), this );
    QWhatsThis::add( label, tr( "Enter the absolute directory path of this entry here." ) );
    layout->addWidget( label );

    QHBoxLayout *layout2 = new QHBoxLayout( this, 2, 4 );
    layout->addLayout( layout2 );

    m_location = new QLineEdit( this );
    QWhatsThis::add( m_location, tr( "Enter the absolute directory path of this entry here." ) );
    layout2->addWidget( m_location );
    QPushButton *btn = new QPushButton( Opie::Core::OResource::loadPixmap( "folder", Opie::Core::OResource::SmallIcon ),
                                        QString::null, this );
    btn->setMaximumWidth( btn->height() );
    QWhatsThis::add( btn, tr( "Tap here to select the desired location." ) );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotSelectPath()) );
    layout2->addWidget( btn );

    // Populate initial information
    if ( m_dest )
    {
        m_name->setText( m_dest->name() );
        m_location->setText( m_dest->value() );
        m_active->setChecked( m_dest->active() );
    }
}

void OIpkgDestDlg::accept()
{
    // Save information entered
    QString name = m_name->text();
    name.replace( QRegExp( " " ), "_" );
    m_dest->setName( name );
    m_dest->setValue( m_location->text() );
    m_dest->setActive( m_active->isChecked() );

    QDialog::accept();
}

void OIpkgDestDlg::slotSelectPath()
{
    QString path = Opie::Ui::OFileDialog::getDirectory( 0, m_location->text() );
    if ( path.at( path.length() - 1 ) == '/' )
        path.truncate( path.length() - 1 );
    if ( !path.isNull() )
        m_location->setText( path );
}

