/*
                            This file is part of the OPIE Project

               =.            Copyright (c)  2003 Dan Williams <drw@handhelds.org>
             .=l.
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "mainwindow.h"
#include "installdlg.h"
#include "filterdlg.h"
#include "promptdlg.h"
#include "entrydlg.h"
#include "packageinfodlg.h"

#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>

#include <qaction.h>
#include <qdir.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qtoolbar.h>
#include <qwhatsthis.h>

MainWindow::MainWindow( QWidget *parent, const char *name, WFlags /*fl*/ )
    : QMainWindow( parent, name, WStyle_ContextHelp )
    , m_config( "packman" )
    , m_packman( &m_config, this )
    , m_menuBar( this )
    , m_toolBar( this )
    , m_findBar( this )
    , m_widgetStack( this )
    , m_packageList( this )
    , m_statusWidget( this )
    , m_statusText( &m_statusWidget )
    , m_statusBar( &m_statusWidget )
    , m_iconUpdated( Resource::loadPixmap( "packagemanager/updated" ) )
    , m_iconInstalled( Resource::loadPixmap( "installed" ) )
    , m_iconNull( m_iconUpdated.size() )
    , m_filterName( QString::null )
    , m_filterServer( QString::null )
    , m_filterDest( QString::null )
    , m_filterStatus( OPackageManager::NotDefined )
    , m_filterCategory( QString::null )

{
//    setCaption( tr( "Package Manager" ) );

    m_iconNull.fill( colorGroup().base() );

    connect( &m_widgetStack, SIGNAL(aboutToShow(QWidget*)), this, SLOT(slotWidgetStackShow(QWidget*)) );

    // Initialize widget stack, package list and status widget
    initStatusWidget();
    initPackageList();

    m_widgetStack.addWidget( &m_statusWidget, 2 );
    m_widgetStack.addWidget( &m_packageList, 1 );
    setCentralWidget( &m_widgetStack );

    // Initialize remaining user interface items
    initUI();

    // Initialize package information
    QTimer::singleShot( 100, this, SLOT( initPackageInfo() ) );
}

void MainWindow::closeEvent( QCloseEvent *event )
{
    // Close app only if either the package or status widgets are currently active
    bool close = m_widgetStack.visibleWidget() == &m_packageList ||
                 m_widgetStack.visibleWidget() == &m_statusWidget;
    if ( close )
    {
        // TODO - write out application configuration settings

        // Write out package manager configuration settings
        m_packman.saveSettings();
        event->accept();
    }
    else
    {
        delete m_widgetStack.visibleWidget();
        m_widgetStack.raiseWidget( &m_packageList );
        event->ignore();
    }
}

void MainWindow::initPackageList()
{
    m_packageList.addColumn( tr( "Packages" ) );
    QWhatsThis::add( &m_packageList, tr( "This is a listing of all packages.\n\nA blue dot next to the package name indicates that the package is currently installed.\n\nA blue dot with a star indicates that a newer version of the package is available from the server feed.\n\nTap inside the box at the left to select a package.  Tap and hold to view package details." ) );
    QPEApplication::setStylusOperation( m_packageList.viewport(), QPEApplication::RightOnHold );
    connect( &m_packageList, SIGNAL(rightButtonPressed(QListViewItem*,const QPoint&,int)),
             this, SLOT(slotDisplayPackageInfo(QListViewItem*)) );
}

void MainWindow::initStatusWidget()
{
    QVBoxLayout *layout = new QVBoxLayout( &m_statusWidget, 4, 4 );

    m_statusText.setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
    layout->addWidget( &m_statusText );

    connect( &m_packman, SIGNAL(initStatus(int)), this, SLOT(slotInitStatusBar(int)) );
    connect( &m_packman, SIGNAL(statusText(const QString&)), this, SLOT(slotStatusText(const QString&)) );
    connect( &m_packman, SIGNAL(statusBar(int)), this, SLOT(slotStatusBar(int)) );

    layout->addWidget( &m_statusBar );
}

void MainWindow::initUI()
{
    // Build menu and tool bars
    setToolBarsMovable( false );

    m_menuBar.setHorizontalStretchable( true );
    QMenuBar *mb = new QMenuBar( &m_menuBar );
    mb->setMargin( 0 );

    // Find toolbar
    addToolBar( &m_findBar, QMainWindow::Top, true );
    m_findBar.setHorizontalStretchable( true );
    m_findEdit = new QLineEdit( &m_findBar );
    QWhatsThis::add( m_findEdit, tr( "Type the text to search for here." ) );
    m_findBar.setStretchableWidget( m_findEdit );
    connect( m_findEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotFindChanged(const QString&)) );

    // Packages menu
    QPopupMenu *popup = new QPopupMenu( this );

    QAction *a = new QAction( tr( "Update lists" ), Resource::loadPixmap( "packagemanager/update" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Tap here to update package lists from servers." ) );
    connect( a, SIGNAL(activated()), this, SLOT(slotUpdate()) );
    a->addTo( popup );
    a->addTo( &m_toolBar );

    QAction *actionUpgrade = new QAction( tr( "Upgrade" ), Resource::loadPixmap( "packagemanager/upgrade" ), QString::null, 0, this, 0 );
    actionUpgrade->setWhatsThis( tr( "Tap here to upgrade all installed packages if a newer version is available." ) );
    connect( actionUpgrade, SIGNAL(activated()), this, SLOT(slotUpgrade()) );
    actionUpgrade->addTo( popup );
    actionUpgrade->addTo( &m_toolBar );

    QPixmap iconDownload = Resource::loadPixmap( "packagemanager/download" );
    QPixmap iconRemove = Resource::loadPixmap( "packagemanager/remove" );
    QAction *actionDownload = new QAction( tr( "Download" ), iconDownload, QString::null, 0, this, 0 );
    actionDownload->setWhatsThis( tr( "Tap here to download the currently selected package(s)." ) );
    connect( actionDownload, SIGNAL(activated()), this, SLOT(slotDownload()) );
    actionDownload->addTo( popup );
    actionDownload->addTo( &m_toolBar );

    a = new QAction( tr( "Apply changes" ), Resource::loadPixmap( "packagemanager/apply" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Tap here to install, remove or upgrade currently selected package(s)." ) );
    connect( a, SIGNAL(activated()), this, SLOT(slotApply()) );
    a->addTo( popup );
    a->addTo( &m_toolBar );

    popup->insertSeparator();

    a = new QAction( tr( "Configure" ), Resource::loadPixmap( "SettingsIcon" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Tap here to configure this application." ) );
    connect( a, SIGNAL(activated()), this, SLOT(slotConfigure()) );
    a->addTo( popup );
    mb->insertItem( tr( "Actions" ), popup );

    // View menu
    popup = new QPopupMenu( this );

    m_actionShowNotInstalled = new QAction( tr( "Show packages not installed" ), QString::null, 0, this, 0 );
    m_actionShowNotInstalled->setToggleAction( true );
    m_actionShowNotInstalled->setWhatsThis( tr( "Tap here to show packages available which have not been installed." ) );
    connect( m_actionShowNotInstalled, SIGNAL(activated()), this, SLOT(slotShowNotInstalled()) );
    m_actionShowNotInstalled->addTo( popup );

    m_actionShowInstalled = new QAction( tr( "Show installed packages" ), QString::null, 0, this, 0 );
    m_actionShowInstalled->setToggleAction( true );
    m_actionShowInstalled->setWhatsThis( tr( "Tap here to show packages currently installed on this device." ) );
    connect( m_actionShowInstalled, SIGNAL(activated()), this, SLOT(slotShowInstalled()) );
    m_actionShowInstalled->addTo( popup );

    m_actionShowUpdated = new QAction( tr( "Show updated packages" ), QString::null, 0, this, 0 );
    m_actionShowUpdated->setToggleAction( true );
    m_actionShowUpdated->setWhatsThis( tr( "Tap here to show packages currently installed on this device which have a newer version available." ) );
    connect( m_actionShowUpdated, SIGNAL(activated()), this, SLOT(slotShowUpdated()) );
    m_actionShowUpdated->addTo( popup );

    popup->insertSeparator();

    m_actionFilter = new QAction( tr( "Filter" ), Resource::loadPixmap( "packagemanager/filter" ),
                                         QString::null, 0, this, 0 );
    m_actionFilter->setToggleAction( true );
    m_actionFilter->setWhatsThis( tr( "Tap here to apply current filter." ) );
    connect( m_actionFilter, SIGNAL(toggled(bool)), this, SLOT(slotFilter(bool)) );
    m_actionFilter->addTo( popup );

    a = new QAction( tr( "Filter settings" ),  QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Tap here to change the package filter criteria." ) );
    connect( a, SIGNAL(activated()), this, SLOT(slotFilterChange()) );
    a->addTo( popup );

    popup->insertSeparator();

    a = new QAction( tr( "Find" ), Resource::loadPixmap( "find" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Tap here to search for text in package names." ) );
    connect( a, SIGNAL(activated()), this, SLOT(slotFindShowToolbar()) );
    a->addTo( popup );

    m_actionFindNext = new QAction( tr( "Find next" ), Resource::loadIconSet( "next" ), QString::null, 0, this, 0 );
    m_actionFindNext->setEnabled( false );
    m_actionFindNext->setWhatsThis( tr( "Tap here to find the next package name containing the text you are searching for." ) );
    connect( m_actionFindNext, SIGNAL(activated()), this, SLOT(slotFindNext()) );
    m_actionFindNext->addTo( popup );
    m_actionFindNext->addTo( &m_findBar );

    mb->insertItem( tr( "View" ), popup );

    // Finish find toolbar creation
    a = new QAction( QString::null, Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Tap here to hide the find toolbar." ) );
    connect( a, SIGNAL(activated()), this, SLOT(slotFindHideToolbar()) );
    a->addTo( &m_findBar );
    m_findBar.hide();
}

void MainWindow::loadPackageList( OPackageList *packages, bool clearList )
{
    if ( clearList )
        m_packageList.clear();

    if ( packages )
    {
        for ( OPackageListIterator packageIt( *packages ); packageIt.current(); ++packageIt )
        {
            OPackage *package = packageIt.current();
            QCheckListItem *item = new QCheckListItem( &m_packageList, package->name(),
                                                    QCheckListItem::CheckBox );
            m_packageList.insertItem( item );

            // If a different version of package is available, show update available icon
            // Otherwise, show installed icon
            if ( !package->versionInstalled().isNull() )
            {
                if ( m_packman.compareVersions( package->version(), package->versionInstalled() ) == 1 )
                    item->setPixmap( 0, m_iconUpdated );
                else
                    item->setPixmap( 0, m_iconInstalled );
            }
            else
                item->setPixmap( 0, m_iconNull );
        }
    }
}

void MainWindow::searchForPackage( const QString &text )
{
    if ( !text.isEmpty() )
    {
        // look through package list for text startng at current position
        QCheckListItem *start = static_cast<QCheckListItem *>(m_packageList.currentItem());
        if ( start == 0 )
            start = static_cast<QCheckListItem *>(m_packageList.firstChild());

//        for ( QCheckListItem *item = static_cast<QCheckListItem *>(start->nextSibling()); item != 0 ;
        for ( QCheckListItem *item = static_cast<QCheckListItem *>(start); item != 0 ;
              item = static_cast<QCheckListItem *>(item->nextSibling()) )
        {
            if ( item->text().lower().find( text ) != -1 )
            {
                m_packageList.ensureItemVisible( item );
                m_packageList.setCurrentItem( item );
                break;
            }
        }
    }
}

void MainWindow::initPackageInfo()
{
    m_widgetStack.raiseWidget( &m_statusWidget );

    // Load package list
    m_packman.loadAvailablePackages();
    m_packman.loadInstalledPackages();

    OPackageList *packageList = m_packman.packages();
    if ( packageList )
    {
        loadPackageList( packageList, true );
        delete packageList;
    }

    m_widgetStack.raiseWidget( &m_packageList );
}

void MainWindow::slotWidgetStackShow( QWidget *widget )
{
    if ( widget == &m_packageList )
    {
        setCaption( tr( "Package Manager" ) );

        m_menuBar.show();
        m_toolBar.show();
    }
    else
    {
        m_menuBar.hide();
        m_toolBar.hide();
    }
}

void MainWindow::slotInitStatusBar( int numSteps )
{
    m_statusBar.setTotalSteps( numSteps );
}

void MainWindow::slotStatusText( const QString &status )
{
    m_statusText.setText( status );
}

void MainWindow::slotStatusBar( int currStep )
{
    m_statusBar.setProgress( currStep );
}

void MainWindow::slotUpdate()
{
    // Create package manager output widget
    InstallDlg *dlg = new InstallDlg( this, &m_packman, tr( "Update package information" ), false,
                                      OPackage::Update );
    connect( dlg, SIGNAL(closeInstallDlg()), this, SLOT(slotCloseDlg()) );

    // Display widget
    m_widgetStack.addWidget( dlg, 3 );
    m_widgetStack.raiseWidget( dlg );
}

void MainWindow::slotUpgrade()
{
    // Create package manager output widget
    InstallDlg *dlg = new InstallDlg( this, &m_packman, tr( "Upgrade installed packages" ), false,
                                      OPackage::Upgrade );
    connect( dlg, SIGNAL(closeInstallDlg()), this, SLOT(slotCloseDlg()) );

    // Display widget
    m_widgetStack.addWidget( dlg, 3 );
    m_widgetStack.raiseWidget( dlg );
}

void MainWindow::slotDownload()
{
    // Retrieve list of packages selected for download (if any)
    QStringList *workingPackages = new QStringList();

    for ( QCheckListItem *item = static_cast<QCheckListItem *>(m_packageList.firstChild());
          item != 0 ;
          item = static_cast<QCheckListItem *>(item->nextSibling()) )
    {
        if ( item->isOn() )
            workingPackages->append( item->text() );
    }

    if ( workingPackages->isEmpty() )
    {
        QMessageBox::information( this, tr( "Nothing to do" ), tr( "No packages selected" ), tr( "OK" ) );
        return;
    }
    else
    {
        // Download selected packages
        m_config.setGroup( "settings" );
        QString workingDir = m_config.readEntry( "DownloadDir", "/tmp" );

        bool ok = false;
        QString text = EntryDlg::getText( tr( "Download" ), tr( "Enter path to download package to:" ), workingDir, &ok, this );
        if ( ok && !text.isEmpty() )
            workingDir = text;   // user entered something and pressed ok
        else
            return; // user entered nothing or pressed cancel

        // Store download directory in config file
        m_config.writeEntry( "DownloadDir", workingDir );

        // Get starting directory
        QDir::setCurrent( workingDir );

        // Create package manager output widget
        InstallDlg *dlg = new InstallDlg( this, &m_packman, tr( "Download packages" ), false,
                                        OPackage::Download, workingPackages );
        connect( dlg, SIGNAL(closeInstallDlg()), this, SLOT(slotCloseDlg()) );

        // Display widget
        m_widgetStack.addWidget( dlg, 3 );
        m_widgetStack.raiseWidget( dlg );
    }
}

void MainWindow::slotApply()
{
    QStringList *removeList = 0x0;
    QStringList *installList = 0x0;
    QStringList *upgradeList = 0x0;

    for ( QCheckListItem *item = static_cast<QCheckListItem *>(m_packageList.firstChild());
          item != 0 ;
          item = static_cast<QCheckListItem *>(item->nextSibling()) )
    {
        if ( item->isOn() )
        {
            OPackage *package = m_packman.findPackage( item->text() );
            if ( package )
            {
                if ( !package->versionInstalled().isNull() )
                {
                    if ( m_packman.compareVersions( package->version(), package->versionInstalled() ) == 1 )
                    {
                        // Remove/upgrade package
                        int answer =  PromptDlg::ask( tr( "Remove or upgrade" ),
                                      tr( QString( "Do you wish to remove or upgrade\n%1?" ).arg( item->text() ) ),
                                      tr( "Remove" ), tr( "Upgrade" ), this );
                        if ( answer == 1 )  // Remove
                        {
                            if ( !removeList )
                                removeList = new QStringList();
                            removeList->append( item->text() );
                        }
                        else if ( answer == 2 )  // Upgrade
                        {
                            if ( !upgradeList )
                                upgradeList = new QStringList();
                            upgradeList->append( item->text() );
                        }
                    }
                    else
                    {
                        // Remove/reinstall package
                        int answer =  PromptDlg::ask( tr( "Remove or reinstall" ),
                                      tr( QString( "Do you wish to remove or reinstall\n%1?" ).arg( item->text() ) ),
                                      tr( "Remove" ), tr( "Reinstall" ), this );
                        if ( answer == 1 )  // Remove
                        {
                            if ( !removeList )
                                removeList = new QStringList();
                            removeList->append( item->text() );
                        }
                        else if ( answer == 2 )  // Reinstall
                        {
                            if ( !installList )
                                installList = new QStringList();
                            installList->append( item->text() );
                        }
                    }
                }
                else
                {
                    // Install package
                    if ( !installList )
                        installList = new QStringList();
                    installList->append( item->text() );
                }
            }
        }
    }

    // If nothing is selected, display message and exit
    if ( !removeList && !installList && !upgradeList )
    {
        QMessageBox::information( this, tr( "Nothing to do" ), tr( "No packages selected" ), tr( "OK" ) );
        return;
    }

    // Send command only if there are packages to process
    OPackage::Command removeCmd = OPackage::NotDefined;
    if ( removeList && !removeList->isEmpty() )
        removeCmd = OPackage::Remove;
    OPackage::Command installCmd = OPackage::NotDefined;
    if ( installList && !installList->isEmpty() )
        installCmd = OPackage::Install;
    OPackage::Command upgradeCmd = OPackage::NotDefined;
    if ( upgradeList && !upgradeList->isEmpty() )
        upgradeCmd = OPackage::Upgrade;

    // Create package manager output widget
    InstallDlg *dlg = new InstallDlg( this, &m_packman, tr( "Apply changes" ), true,
                                      removeCmd, removeList,
                                      installCmd, installList,
                                      upgradeCmd, upgradeList );
    connect( dlg, SIGNAL(closeInstallDlg()), this, SLOT(slotCloseDlg()) );

    // Display widget
    m_widgetStack.addWidget( dlg, 3 );
    m_widgetStack.raiseWidget( dlg );
}

void MainWindow::slotCloseDlg()
{
    // Close install dialog
    delete m_widgetStack.visibleWidget();

    // Reload package list
    initPackageInfo();

    // Update Opie launcher links
    QCopEnvelope e("QPE/System", "linkChanged(QString)");
    QString lf = QString::null;
    e << lf;
}

void MainWindow::slotConfigure()
{
    if ( m_packman.configureDlg( false ) )
    {
        if ( PromptDlg::ask( tr( "Config updated" ),
             tr( "The configuration has been updated.  Do you want to update server and package information now?" ),
             tr( "Yes" ), tr( "No" ), this ) == 1 )
        {
            // Update package list and reload package info
            slotUpdate();
        }
    }
}

void MainWindow::slotShowNotInstalled()
{
    OPackageList *packageList;
    if ( m_actionShowNotInstalled->isOn() )
    {
        m_actionShowInstalled->setOn( false );
        m_actionShowUpdated->setOn( false );
        m_actionFilter->setOn( false );
        packageList = m_packman.filterPackages( QString::null, QString::null, QString::null,
                                                OPackageManager::NotInstalled, QString::null );
    }
    else
        packageList = m_packman.packages();

    if ( packageList )
    {
        loadPackageList( packageList, true );
        delete packageList;
    }
}

void MainWindow::slotShowInstalled()
{
    OPackageList *packageList;
    if ( m_actionShowInstalled->isOn() )
    {
        m_actionShowNotInstalled->setOn( false );
        m_actionShowUpdated->setOn( false );
        m_actionFilter->setOn( false );
        packageList = m_packman.filterPackages( QString::null, QString::null, QString::null,
                                                OPackageManager::Installed, QString::null );
    }
    else
        packageList = m_packman.packages();

    if ( packageList )
    {
        loadPackageList( packageList, true );
        delete packageList;
    }
}

void MainWindow::slotShowUpdated()
{
    OPackageList *packageList;
    if ( m_actionShowUpdated->isOn() )
    {
        m_actionShowInstalled->setOn( false );
        m_actionShowNotInstalled->setOn( false );
        m_actionFilter->setOn( false );
        packageList = m_packman.filterPackages( QString::null, QString::null, QString::null,
                                                OPackageManager::Updated, QString::null );
    }
    else
        packageList = m_packman.packages();

    if ( packageList )
    {
        loadPackageList( packageList, true );
        delete packageList;
    }
}

void MainWindow::slotFilterChange()
{
    FilterDlg dlg( this, &m_packman, m_filterName, m_filterServer, m_filterDest, m_filterStatus,
                   m_filterCategory );
    if ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted )
    {
        m_filterName = dlg.name();
        m_filterServer = dlg.server();
        m_filterDest = dlg.destination();
        m_filterStatus = dlg.status();
        m_filterCategory = dlg.category();
        m_actionFilter->setOn( true );
        slotFilter( true );
    }
    else
    {
        m_actionFilter->setOn( false );
        slotFilter( false );
    }
}

void MainWindow::slotFilter( bool isOn )
{
    OPackageList *packageList;
    if ( isOn )
    {
        // Turn off other filtering options
        m_actionShowNotInstalled->setOn( false );
        m_actionShowInstalled->setOn( false );
        m_actionShowUpdated->setOn( false );

        // If the filter settings have not been set yet, display filter dialog
        if ( m_filterName.isNull() && m_filterServer.isNull() && m_filterDest.isNull() &&
             m_filterStatus == OPackageManager::NotDefined && m_filterCategory.isNull() )
        {
            FilterDlg dlg( this, &m_packman, m_filterName, m_filterServer, m_filterDest, m_filterStatus,
                        m_filterCategory );
            if ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted )
            {
                m_filterName = dlg.name();
                m_filterServer = dlg.server();
                m_filterDest = dlg.destination();
                m_filterStatus = dlg.status();
                m_filterCategory = dlg.category();
                m_actionFilter->setOn( true );
                packageList = m_packman.filterPackages( m_filterName, m_filterServer, m_filterDest,
                                                        m_filterStatus, m_filterCategory );
            }
            else
            {
                m_actionFilter->setOn( false );
                packageList = m_packman.packages();
            }
        }
        else
        {
            m_actionFilter->setOn( true );
            packageList = m_packman.filterPackages( m_filterName, m_filterServer, m_filterDest,
                                                    m_filterStatus, m_filterCategory );
        }


    }
    else
        packageList = m_packman.packages();

    if ( packageList )
    {
        loadPackageList( packageList, true );
        delete packageList;
    }
}

void MainWindow::slotFindShowToolbar()
{
    m_findBar.show();
    m_findEdit->setFocus();
}

void MainWindow::slotFindHideToolbar()
{
    m_findBar.hide();
}

void MainWindow::slotFindChanged( const QString &findText )
{

    m_actionFindNext->setEnabled( !findText.isEmpty() );
    searchForPackage( findText );
}

void MainWindow::slotFindNext()
{
    searchForPackage( m_findEdit->text() );
}

void MainWindow::slotDisplayPackageInfo( QListViewItem *packageItem )
{
    QString packageName( ( static_cast<QCheckListItem*>( packageItem ) )->text() );

    // Create package manager output widget
    PackageInfoDlg *dlg = new PackageInfoDlg( this, &m_packman, packageName );

    // Display widget
    m_widgetStack.addWidget( dlg, 3 );
    m_widgetStack.raiseWidget( dlg );
}
