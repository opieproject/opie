/*
                    This file is part of the Opie Project

                      Copyright (c)  2003 Dan Williams <drw@handhelds.org>
              =.
            .=l.
     .>+-=
_;:,   .>  :=|.         This program is free software; you can
.> <`_,  > .  <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--  :           the terms of the GNU Library General Public
.="- .-=="i,   .._         License as published by the Free Software
- .  .-<_>   .<>         Foundation; either version 2 of the License,
  ._= =}    :          or (at your option) any later version.
  .%`+i>    _;_.
  .i_,=:_.   -<s.       This program is distributed in the hope that
  + . -:.    =       it will be useful,  but WITHOUT ANY WARRANTY;
  : ..  .:,   . . .    without even the implied warranty of
  =_    +   =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.    :  :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=    =    ;      Library General Public License for more
++=  -.   .`   .:       details.
:   = ...= . :.=-
-.  .:....=;==+<;          You should have received a copy of the GNU
 -_. . .  )=. =           Library General Public License along with
  --    :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "oipkgconfigdlg.h"

#include <opie2/ofiledialog.h>

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>

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
    , m_serverNew( false )
    , m_serverCurrent( -1 )
    , m_destNew( false )
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
        OConfItem *confItem = findConfItem( OConfItem::Option, "http_proxy" );
        if ( confItem )
        {
            confItem->setValue( m_proxyHttpServer->text() );
            confItem->setActive( m_proxyHttpActive->isChecked() );
        }
        else
            m_configs->append( new OConfItem( OConfItem::Option, "http_proxy",
                               m_proxyHttpServer->text(), QString::null,
                               m_proxyHttpActive->isChecked() ) );

        confItem = findConfItem( OConfItem::Option, "ftp_proxy" );
        if ( confItem )
        {
            confItem->setValue( m_proxyFtpServer->text() );
            confItem->setActive( m_proxyFtpActive->isChecked() );
        }
        else
            m_configs->append( new OConfItem( OConfItem::Option, "ftp_proxy",
                               m_proxyFtpServer->text(), QString::null,
                               m_proxyFtpActive->isChecked() ) );

        confItem = findConfItem( OConfItem::Option, "proxy_username" );
        if ( confItem )
            confItem->setValue( m_proxyUsername->text() );
        else
            m_configs->append( new OConfItem( OConfItem::Option, "proxy_username",
                               m_proxyUsername->text() ) );

        confItem = findConfItem( OConfItem::Option, "proxy_password" );
        if ( confItem )
            confItem->setValue( m_proxyPassword->text() );
        else
            m_configs->append( new OConfItem( OConfItem::Option, "proxy_password",
                               m_proxyPassword->text() ) );

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
    layout->addMultiCellWidget( m_serverList, 0, 0, 0, 1 );

    QPushButton *btn = new QPushButton( Resource::loadPixmap( "new" ), tr( "New" ), container );
    QWhatsThis::add( btn, tr( "Tap here to create a new entry.  Fill in the fields below and then tap on Update." ) );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotServerNew()) );
    layout->addWidget( btn, 1, 0 );

    m_serverEditBtn = new QPushButton( Resource::loadPixmap( "edit" ), tr( "Edit" ), container );
    m_serverEditBtn->setEnabled( false );
    QWhatsThis::add( m_serverEditBtn, tr( "Tap here to edit the entry selected above." ) );
    connect( m_serverEditBtn, SIGNAL(clicked()), this, SLOT(slotServerEdit()) );
    layout->addWidget( m_serverEditBtn, 1, 1 );
    
    m_serverDeleteBtn = new QPushButton( Resource::loadPixmap( "trash" ), tr( "Delete" ), container );
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
    QGridLayout *layout = new QGridLayout( container, 3, 2, 2, 4 );

    m_destList = new QListBox( container );
    QWhatsThis::add( m_destList, tr( "This is a list of all destinations configured for this device.  Select one here to edit or delete, or add a new one below." ) );
    m_destList->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    connect( m_destList, SIGNAL(highlighted(int)), this, SLOT(slotDestEdit(int)) );
    layout->addMultiCellWidget( m_destList, 0, 0, 0, 1 );

    QPushButton *btn = new QPushButton( Resource::loadPixmap( "new" ), tr( "New" ), container );
    QWhatsThis::add( btn, tr( "Tap here to create a new entry.  Fill in the fields below and then tap on Update." ) );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotDestNew()) );
    layout->addWidget( btn, 1, 0 );

    btn = new QPushButton( Resource::loadPixmap( "trash" ), tr( "Delete" ), container );
    QWhatsThis::add( btn, tr( "Tap here to delete the entry selected above." ) );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotDestDelete()) );
    layout->addWidget( btn, 1, 1 );

    QGroupBox *grpbox = new QGroupBox( 0, Qt::Vertical, tr( "Destination" ), container );
    grpbox->layout()->setSpacing( 2 );
    grpbox->layout()->setMargin( 4 );
    layout->addMultiCellWidget( grpbox, 2, 2, 0, 1 );

    QGridLayout *grplayout = new QGridLayout( grpbox->layout() );

    QLabel *label = new QLabel( tr( "Name:" ), grpbox );
    QWhatsThis::add( label, tr( "Enter the name of this entry here." ) );
    grplayout->addWidget( label, 0, 0 );
    m_destName = new QLineEdit( grpbox );
    QWhatsThis::add( m_destName, tr( "Enter the name of this entry here." ) );
    grplayout->addMultiCellWidget( m_destName, 0, 0, 1, 2 );

    label = new QLabel( tr( "Location:" ), grpbox );
    QWhatsThis::add( label, tr( "Enter the absolute directory path of this entry here." ) );
    grplayout->addWidget( label, 1, 0 );
    m_destLocation = new QLineEdit( grpbox );
    QWhatsThis::add( m_destLocation, tr( "Enter the absolute directory path of this entry here." ) );
    grplayout->addWidget( m_destLocation, 1, 1 );
    btn = new QPushButton( Resource::loadPixmap( "folder" ), QString::null, grpbox );
    btn->setMaximumWidth( btn->height() );
    QWhatsThis::add( btn, tr( "Tap here to select the desired location." ) );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotDestSelectPath()) );
    grplayout->addWidget( btn, 1, 2 );

    m_destActive = new QCheckBox( tr( "Active" ), grpbox );
    QWhatsThis::add( m_destActive, tr( "Tap here to indicate whether this entry is active or not." ) );
    grplayout->addMultiCellWidget( m_destActive, 2, 2, 0, 2 );

    btn = new QPushButton( Resource::loadPixmap( "edit" ), tr( "Update" ), grpbox );
    QWhatsThis::add( btn, tr( "Tap here to update the entry's information." ) );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotDestUpdate()) );
    grplayout->addMultiCellWidget( btn, 3, 3, 0, 2 );
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
    QVBoxLayout *layout = new QVBoxLayout( container, 2, 4 );

    m_optForceDepends = new QCheckBox( tr( "Force Depends" ), container );
    QWhatsThis::add( m_optForceDepends, tr( "Tap here to enable or disable the '-force-depends' option for Ipkg." ) );
    layout->addWidget( m_optForceDepends );

    m_optForceReinstall = new QCheckBox( tr( "Force Reinstall" ), container );
    QWhatsThis::add( m_optForceReinstall, tr( "Tap here to enable or disable the '-force-reinstall' option for Ipkg." ) );
    layout->addWidget( m_optForceReinstall );

    m_optForceRemove = new QCheckBox( tr( "Force Remove" ), container );
    QWhatsThis::add( m_optForceRemove, tr( "Tap here to enable or disable the '-force-removal-of-dependent-packages' option for Ipkg." ) );
    layout->addWidget( m_optForceRemove );

    m_optForceOverwrite = new QCheckBox( tr( "Force Overwrite" ), container );
    QWhatsThis::add( m_optForceOverwrite, tr( "Tap here to enable or disable the '-force-overwrite' option for Ipkg." ) );
    layout->addWidget( m_optForceOverwrite );

    QLabel *l = new QLabel( tr( "Information Level" ), container );
    QWhatsThis::add( l, tr( "Select information level for Ipkg." ) );
    layout->addWidget( l );

    m_optVerboseIpkg = new QComboBox( container );
    QWhatsThis::add( m_optVerboseIpkg, tr( "Select information level for Ipkg." ) );
    m_optVerboseIpkg->insertItem( tr( "Errors only" ) );
    m_optVerboseIpkg->insertItem( tr( "Normal messages" ) );
    m_optVerboseIpkg->insertItem( tr( "Informative messages" ) );
    m_optVerboseIpkg->insertItem( tr( "Troubleshooting output" ) );
    layout->addWidget( m_optVerboseIpkg );

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
                    if ( config->type() == OConfItem::Source )
                    {
                        m_serverList->insertItem( config->name() );
                    }
                    else if ( config->type() == OConfItem::Destination )
                    {
                        m_destList->insertItem( config->name() );
                    }
                    else if ( config->type() == OConfItem::Option )
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

    m_optVerboseIpkg->setCurrentItem( m_ipkg->ipkgExecVerbosity() );
}

OConfItem *OIpkgConfigDlg::findConfItem( OConfItem::Type type, const QString &name )
{
    // Find selected server in list
    OConfItemListIterator configIt( *m_configs );
    OConfItem *config = 0l;
    for ( ; configIt.current(); ++configIt )
    {
        config = configIt.current();
        if ( config->type() == type && config->name() == name )
            break;
    }

    if ( config && config->type() == type && config->name() == name )
        return config;

    return 0l;
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
    OConfItem *server = findConfItem( OConfItem::Source, m_serverList->currentText() );

    // Delete server
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
    OConfItem *server = findConfItem( OConfItem::Source, m_serverList->currentText() );

    // Delete server
    if ( server )
    {
        m_configs->removeRef( server );
        m_serverList->removeItem( m_serverCurrent );
    }
}

void OIpkgConfigDlg::slotDestEdit( int index )
{
    m_destNew = false;
    m_destCurrent = index;

    // Find selected destination in list
    OConfItem *destination = findConfItem( OConfItem::Destination, m_destList->currentText() );

    // Display destination details
    if ( destination )
    {
        m_destCurrName = destination->name();
        m_destName->setText( destination->name() );
        m_destLocation->setText( destination->value() );
        m_destActive->setChecked( destination->active() );
        m_destName->setFocus();
    }
}

void OIpkgConfigDlg::slotDestNew()
{
    m_destNew = true;

    m_destName->setText( QString::null );
    m_destLocation->setText( QString::null );
    m_destActive->setChecked( true );
    m_destName->setFocus();
}

void OIpkgConfigDlg::slotDestDelete()
{
    // Find selected destination in list
    OConfItem *destination = findConfItem( OConfItem::Destination, m_destList->currentText() );

    // Delete destination
    if ( destination )
    {
        m_configs->removeRef( destination );
        m_destList->removeItem( m_destCurrent );
    }
}

void OIpkgConfigDlg::slotDestSelectPath()
{
    QString path = Opie::Ui::OFileDialog::getDirectory( 0, m_destLocation->text() );
    if ( path.at( path.length() - 1 ) == '/' )
        path.truncate( path.length() - 1 );
    m_destLocation->setText( path );
}

void OIpkgConfigDlg::slotDestUpdate()
{
    QString newName = m_destName->text();

    // Convert any spaces to underscores
    newName.replace( QRegExp( " " ), "_" );

    if ( !m_destNew )
    {
        // Find selected destination in list
        OConfItem *destination = findConfItem( OConfItem::Destination, m_destCurrName );

        // Display destination details
        if ( destination )
        {
            // Update url
            destination->setValue( m_destLocation->text() );
            destination->setActive( m_destActive->isChecked() );

            // Check if destination name has changed, if it has then we need to replace the key in the map
            if ( m_destCurrName != newName )
            {
                // Update destination name
                destination->setName( newName );

                // Update list box
                m_destList->changeItem( newName, m_destCurrent );
            }
        }
    }
    else
    {
        // Add new destination to configuration list
        m_configs->append( new OConfItem( OConfItem::Destination, newName,
                           m_destLocation->text(), QString::null, m_destActive->isChecked() ) );
        m_configs->sort();

        m_destList->insertItem( newName );
        m_destList->setCurrentItem( m_destList->count() );
        m_destNew = false;
    }
}

OIpkgServerDlg::OIpkgServerDlg( OConfItem *server, QWidget *parent )
    : QDialog( parent, QString::null, true, WStyle_ContextHelp )
    , m_server( server )
{
    setCaption( tr( "Edit server" ) );

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
    m_server->setName( m_name->text() );
    m_server->setValue( m_location->text() );
    m_compressed->isChecked() ? m_server->setFeatures( "Compressed" )
                              : m_server->setFeatures( QString::null );
    m_server->setActive( m_active->isChecked() );

    QDialog::accept();
}
