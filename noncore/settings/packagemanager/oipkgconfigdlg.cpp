/*
                            This file is part of the Opie Project

                             Copyright (c)  2003 Dan Williams <drw@handhelds.org>
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

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qscrollview.h>

#include <qpe/resource.h>

OIpkgConfigDlg::OIpkgConfigDlg( OIpkg *ipkg, bool installOptions, QWidget *parent )
    : QDialog( parent, QString::null, true )
    , m_ipkg( ipkg )
    , m_configs( 0x0 )
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

    //showMaximized();
}

void OIpkgConfigDlg::accept()
{
    // Save server, destination and proxy configuration
    if ( !m_installOptions )
        m_ipkg->setConfigItems( m_configs );

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
    QGridLayout *layout = new QGridLayout( container, 3, 2, 2, 4 );

    m_serverList = new QListBox( container );
    m_serverList->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    connect( m_serverList, SIGNAL(highlighted(int)), this, SLOT(slotServerEdit(int)) );
    layout->addMultiCellWidget( m_serverList, 0, 0, 0, 1 );

    QPushButton *btn = new QPushButton( Resource::loadPixmap( "new" ), tr( "New" ), container );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotServerNew()) );
    layout->addWidget( btn, 1, 0 );

    btn = new QPushButton( Resource::loadPixmap( "trash" ), tr( "Delete" ), container );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotServerDelete()) );
    layout->addWidget( btn, 1, 1 );

    QGroupBox *grpbox = new QGroupBox( 0, Qt::Vertical, tr( "Server" ), container );
    grpbox->layout()->setSpacing( 2 );
    grpbox->layout()->setMargin( 4 );
    layout->addMultiCellWidget( grpbox, 2, 2, 0, 1 );

    QGridLayout *grplayout = new QGridLayout( grpbox->layout() );

    QLabel *label = new QLabel( tr( "Name:" ), grpbox );
    grplayout->addWidget( label, 0, 0 );
    m_serverName = new QLineEdit( grpbox );
    grplayout->addWidget( m_serverName, 0, 1 );

    label = new QLabel( tr( "Address:" ), grpbox );
    grplayout->addWidget( label, 1, 0 );
    m_serverLocation = new QLineEdit( grpbox );
    grplayout->addWidget( m_serverLocation, 1, 1 );

    m_serverActive = new QCheckBox( tr( "Active Server" ), grpbox );
    grplayout->addMultiCellWidget( m_serverActive, 2, 2, 0, 1 );

    btn = new QPushButton( Resource::loadPixmap( "edit" ), tr( "Update" ), grpbox );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotServerUpdate()) );
    grplayout->addMultiCellWidget( btn, 3, 3, 0, 1 );
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
    m_destList->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    connect( m_destList, SIGNAL(highlighted(int)), this, SLOT(slotDestEdit(int)) );
    layout->addMultiCellWidget( m_destList, 0, 0, 0, 1 );

    QPushButton *btn = new QPushButton( Resource::loadPixmap( "new" ), tr( "New" ), container );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotDestNew()) );
    layout->addWidget( btn, 1, 0 );

    btn = new QPushButton( Resource::loadPixmap( "trash" ), tr( "Delete" ), container );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotDestDelete()) );
    layout->addWidget( btn, 1, 1 );

    QGroupBox *grpbox = new QGroupBox( 0, Qt::Vertical, tr( "Server" ), container );
    grpbox->layout()->setSpacing( 2 );
    grpbox->layout()->setMargin( 4 );
    layout->addMultiCellWidget( grpbox, 2, 2, 0, 1 );

    QGridLayout *grplayout = new QGridLayout( grpbox->layout() );

    QLabel *label = new QLabel( tr( "Name:" ), grpbox );
    grplayout->addWidget( label, 0, 0 );
    m_destName = new QLineEdit( grpbox );
    grplayout->addWidget( m_destName, 0, 1 );

    label = new QLabel( tr( "Address:" ), grpbox );
    grplayout->addWidget( label, 1, 0 );
    m_destLocation = new QLineEdit( grpbox );
    grplayout->addWidget( m_destLocation, 1, 1 );

    m_destActive = new QCheckBox( tr( "Active Server" ), grpbox );
    grplayout->addMultiCellWidget( m_destActive, 2, 2, 0, 1 );

    btn = new QPushButton( Resource::loadPixmap( "edit" ), tr( "Update" ), grpbox );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotDestUpdate()) );
    grplayout->addMultiCellWidget( btn, 3, 3, 0, 1 );
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
    grplayout->addWidget( m_proxyHttpServer );
    m_proxyHttpActive = new QCheckBox( tr( "Enabled" ), grpbox );
    grplayout->addWidget( m_proxyHttpActive );

    // FTP proxy server configuration
    grpbox = new QGroupBox( 0, Qt::Vertical, tr( "FTP Proxy" ), container );
    grpbox->layout()->setSpacing( 2 );
    grpbox->layout()->setMargin( 4 );
    layout->addMultiCellWidget( grpbox, 1, 1, 0, 1 );
    grplayout = new QVBoxLayout( grpbox->layout() );
    m_proxyFtpServer = new QLineEdit( grpbox );
    grplayout->addWidget( m_proxyFtpServer );
    m_proxyFtpActive = new QCheckBox( tr( "Enabled" ), grpbox );
    grplayout->addWidget( m_proxyFtpActive );

    // Proxy server username and password configuration
    QLabel *label = new QLabel( tr( "Username:" ), container );
    layout->addWidget( label, 2, 0 );
    m_proxyUsername = new QLineEdit( container );
    layout->addWidget( m_proxyUsername, 2, 1 );

    label = new QLabel( tr( "Password:" ), container );
    layout->addWidget( label, 3, 0 );
    m_proxyPassword = new QLineEdit( container );
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
    layout->addWidget( m_optForceDepends );

    m_optForceReinstall = new QCheckBox( tr( "Force Reinstall" ), container );
    layout->addWidget( m_optForceReinstall );

    m_optForceRemove = new QCheckBox( tr( "Force Remove" ), container );
    layout->addWidget( m_optForceRemove );

    m_optForceOverwrite = new QCheckBox( tr( "Force Overwrite" ), container );
    layout->addWidget( m_optForceOverwrite );

    QLabel *l = new QLabel( tr( "Information Level" ), container );
    layout->addWidget( l );

    m_optVerboseIpkg = new QComboBox( container );
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
    OConfItem *config = 0x0;
    for ( ; configIt.current(); ++configIt )
    {
        config = configIt.current();
        if ( config->type() == type && config->name() == name )
            break;
    }

    if ( config && config->type() == type && config->name() == name )
        return config;

    return 0x0;
}

void OIpkgConfigDlg::slotServerEdit( int index )
{
    m_serverNew = false;
    m_serverCurrent = index;

    // Find selected server in list
    OConfItem *server = findConfItem( OConfItem::Source, m_serverList->currentText() );

    // Display server details
    if ( server )
    {
        m_serverCurrName = server->name();
        m_serverName->setText( server->name() );
        m_serverLocation->setText( server->value() );
        m_serverActive->setChecked( server->active() );
        m_serverName->setFocus();
    }
}

void OIpkgConfigDlg::slotServerNew()
{
    m_serverNew = true;

    m_serverName->setText( QString::null );
    m_serverLocation->setText( QString::null );
    m_serverActive->setChecked( true );
    m_serverName->setFocus();
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

void OIpkgConfigDlg::slotServerUpdate()
{
    QString newName = m_serverName->text();

    // Convert any spaces to underscores
    newName.replace( QRegExp( " " ), "_" );

    if ( !m_serverNew )
    {
        // Find selected server in list
        OConfItem *server = findConfItem( OConfItem::Source, m_serverCurrName );

        // Delete server
        if ( server )
        {
            // Update url
            server->setValue( m_serverLocation->text() );
            server->setActive( m_serverActive->isChecked() );

            // Check if server name has changed, if it has then we need to replace the key in the map
            if ( m_serverCurrName != newName )
            {
                // Update server name
                server->setName( newName );

                // Update list box
                m_serverList->changeItem( newName, m_serverCurrent );
            }
        }
    }
    else
    {
        // Add new destination to configuration list
        m_configs->append( new OConfItem( QString::null, OConfItem::Source, newName,
                           m_serverLocation->text(), m_serverActive->isChecked() ) );
        m_configs->sort();

        m_serverList->insertItem( newName );
        m_serverList->setCurrentItem( m_serverList->count() );
        m_serverNew = false;
        m_serverList->insertItem( newName );
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
        m_configs->append( new OConfItem( QString::null, OConfItem::Destination, newName,
                           m_destLocation->text(), m_destActive->isChecked() ) );
        m_configs->sort();

        m_destList->insertItem( newName );
        m_destList->setCurrentItem( m_destList->count() );
        m_destNew = false;
    }
}
