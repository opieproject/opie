/*
                             This file is part of the OPIE Project
 
               =.            Copyright (c)  2002 Andy Qua <andy.qua@blueyonder.co.uk>
             .=l.                                Dan Williams <drw@handhelds.org>
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

#include "categoryfilterimpl.h"
#include "datamgr.h"
#include "global.h"
#include "inputdlg.h"
#include "ipkg.h"
#include "installdlgimpl.h"
#include "letterpushbutton.h"
#include "mainwin.h"
#include "packagewin.h"
#include "settingsimpl.h"
#include "utils.h"

/* OPIE */
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/resource.h>

/* QT */
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qaction.h>
#include <qcombobox.h>
#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qprogressbar.h>
#include <qtimer.h>
#include <qwhatsthis.h>
#include <qwidgetstack.h>

/* STD */
#include <linux/limits.h>
#include <unistd.h>

extern int compareVersions( const char *v1, const char *v2 );

MainWindow :: MainWindow( QWidget* parent, const char* name, WFlags fl )
        :	QMainWindow( parent, name, fl || WStyle_ContextHelp )
{
    // Disable suspend mode
    QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::DisableSuspend;

    LOCAL_SERVER = QObject::tr( "Installed packages" );
    LOCAL_IPKGS = QObject::tr( "Local packages" );

    setCaption( tr( "AQPkg - Package Manager" ) );

    // Create UI widgets
    initMainWidget();
    initProgressWidget();

    // Build menu and tool bars
    setToolBarsMovable( FALSE );

    QToolBar *bar = new QToolBar( this );
    bar->setHorizontalStretchable( TRUE );
    QMenuBar *mb = new QMenuBar( bar );
    mb->setMargin( 0 );
    bar = new QToolBar( this );

    // Find toolbar
    findBar = new QToolBar( this );
    addToolBar( findBar, QMainWindow::Top, true );
    findBar->setHorizontalStretchable( true );
    findEdit = new QLineEdit( findBar );
    QWhatsThis::add( findEdit, tr( "Type the text to search for here." ) );
    findBar->setStretchableWidget( findEdit );
    connect( findEdit, SIGNAL( textChanged( const QString & ) ), this, SLOT( findPackage( const QString & ) ) );

    // Quick jump toolbar
    jumpBar = new QToolBar( this );
    addToolBar( jumpBar, QMainWindow::Top, true );
    jumpBar->setHorizontalStretchable( true );
    QWidget *w = new QWidget( jumpBar );
    jumpBar->setStretchableWidget( w );

    QGridLayout *layout = new QGridLayout( w );

    char text[2];
    text[1] = '\0';
    for ( int i = 0 ; i < 26 ; ++i )
    {
        text[0] = 'A' + i;
        LetterPushButton *b = new LetterPushButton( text, w );
        connect( b, SIGNAL( released( QString ) ), this, SLOT( letterPushed( QString ) ) );
        layout->addWidget( b, i / 13, i % 13);
    }

    QAction *a = new QAction( QString::null, Resource::loadPixmap( "close" ), QString::null, 0, w, 0 );
    a->setWhatsThis( tr( "Click here to hide the Quick Jump toolbar." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( hideJumpBar() ) );
    a->addTo( jumpBar );
    jumpBar->hide();

    // Packages menu
    QPopupMenu *popup = new QPopupMenu( this );

    a = new QAction( tr( "Update lists" ), Resource::loadPixmap( "aqpkg/update" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to update package lists from servers." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( updateServer() ) );
    a->addTo( popup );
    a->addTo( bar );

    actionUpgrade = new QAction( tr( "Upgrade" ), Resource::loadPixmap( "aqpkg/upgrade" ), QString::null, 0, this, 0 );
    actionUpgrade->setWhatsThis( tr( "Click here to upgrade all installed packages if a newer version is available." ) );
    connect( actionUpgrade, SIGNAL( activated() ), this, SLOT( upgradePackages() ) );
    actionUpgrade->addTo( popup );
    actionUpgrade->addTo( bar );

    iconDownload = Resource::loadPixmap( "aqpkg/download" );
    iconRemove = Resource::loadPixmap( "aqpkg/remove" );
    actionDownload = new QAction( tr( "Download" ), iconDownload, QString::null, 0, this, 0 );
    actionDownload->setWhatsThis( tr( "Click here to download the currently selected package(s)." ) );
    connect( actionDownload, SIGNAL( activated() ), this, SLOT( downloadPackage() ) );
    actionDownload->addTo( popup );
    actionDownload->addTo( bar );

    a = new QAction( tr( "Apply changes" ), Resource::loadPixmap( "aqpkg/apply" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to install, remove or upgrade currently selected package(s)." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( applyChanges() ) );
    a->addTo( popup );
    a->addTo( bar );

    popup->insertSeparator();

    a = new QAction( tr( "Configure" ), Resource::loadPixmap( "SettingsIcon" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to configure this application." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( displaySettings() ) );
    a->addTo( popup );
    mb->insertItem( tr( "Actions" ), popup );

    // View menu
    popup = new QPopupMenu( this );

    actionUninstalled = new QAction( tr( "Show packages not installed" ), QString::null, 0, this, 0 );
    actionUninstalled->setToggleAction( TRUE );
    actionUninstalled->setWhatsThis( tr( "Click here to show packages available which have not been installed." ) );
    connect( actionUninstalled, SIGNAL( activated() ), this, SLOT( filterUninstalledPackages() ) );
    actionUninstalled->addTo( popup );

    actionInstalled = new QAction( tr( "Show installed packages" ), QString::null, 0, this, 0 );
    actionInstalled->setToggleAction( TRUE );
    actionInstalled->setWhatsThis( tr( "Click here to show packages currently installed on this device." ) );
    connect( actionInstalled, SIGNAL( activated() ), this, SLOT( filterInstalledPackages() ) );
    actionInstalled->addTo( popup );

    actionUpdated = new QAction( tr( "Show updated packages" ), QString::null, 0, this, 0 );
    actionUpdated->setToggleAction( TRUE );
    actionUpdated->setWhatsThis( tr( "Click here to show packages currently installed on this device which have a newer version available." ) );
    connect( actionUpdated, SIGNAL( activated() ), this, SLOT( filterUpgradedPackages() ) );
    actionUpdated->addTo( popup );

    popup->insertSeparator();

    actionFilter = new QAction( tr( "Filter by category" ), Resource::loadPixmap( "aqpkg/filter" ),  QString::null, 0, this, 0 );
    actionFilter->setToggleAction( TRUE );
    actionFilter->setWhatsThis( tr( "Click here to list packages belonging to one category." ) );
    connect( actionFilter, SIGNAL( activated() ), this, SLOT( filterCategory() ) );
    actionFilter->addTo( popup );

    a = new QAction( tr( "Set filter category" ),  QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to change package category to used filter." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( setFilterCategory() ) );
    a->addTo( popup );

    popup->insertSeparator();

    a = new QAction( tr( "Find" ), Resource::loadPixmap( "find" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to search for text in package names." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( displayFindBar() ) );
    a->addTo( popup );

    actionFindNext = new QAction( tr( "Find next" ), Resource::loadIconSet( "next" ), QString::null, 0, this, 0 );
    actionFindNext->setEnabled( FALSE );
    actionFindNext->setWhatsThis( tr( "Click here to find the next package name containing the text you are searching for." ) );
    connect( actionFindNext, SIGNAL( activated() ), this, SLOT( repeatFind() ) );
    actionFindNext->addTo( popup );
    actionFindNext->addTo( findBar );


    popup->insertSeparator();

    a = new QAction( tr( "Quick Jump keypad" ), Resource::loadPixmap( "aqpkg/keyboard" ),  QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to display/hide keypad to allow quick movement through the package list." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( displayJumpBar() ) );
    a->addTo( popup );

    mb->insertItem( tr( "View" ), popup );

    // Finish find toolbar creation
    a = new QAction( QString::null, Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to hide the find toolbar." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( hideFindBar() ) );
    a->addTo( findBar );
    findBar->hide();

    // Create widget stack and add UI widgets
    stack = new QWidgetStack( this );
    stack->addWidget( progressWindow, 2 );
    stack->addWidget( networkPkgWindow, 1 );
    setCentralWidget( stack );
    stack->raiseWidget( progressWindow );

    // Delayed call to finish initialization
    QTimer::singleShot( 100, this, SLOT( init() ) );
}

MainWindow :: ~MainWindow()
{
    delete mgr;

    // Reenable suspend mode
    QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
}

void MainWindow :: initMainWidget()
{
    networkPkgWindow = new QWidget( this );

    QLabel *l = new QLabel( tr( "Servers:" ), networkPkgWindow );

    serversList = new QComboBox( networkPkgWindow );
    connect( serversList, SIGNAL(activated(int)), this, SLOT(serverSelected(int)) );
    QWhatsThis::add( serversList, tr( "Click here to select a package feed." ) );

    installedIcon = Resource::loadPixmap( "installed" );
    updatedIcon = Resource::loadPixmap( "aqpkg/updated" );

    packagesList = new QListView( networkPkgWindow );
    packagesList->addColumn( tr( "Packages" ), 225 );
    QWhatsThis::add( packagesList, tr( "This is a listing of all packages for the server feed selected above.\n\nA blue dot next to the package name indicates that the package is currently installed.\n\nA blue dot with a star indicates that a newer version of the package is available from the server feed.\n\nClick inside the box at the left to select a package." ) );
    QPEApplication::setStylusOperation( packagesList->viewport(), QPEApplication::RightOnHold );
    connect( packagesList, SIGNAL(rightButtonPressed(QListViewItem *,const QPoint &,int)),
             this, SLOT(slotDisplayPackage(QListViewItem *)) );

    QVBoxLayout *vbox = new QVBoxLayout( networkPkgWindow, 0, -1 );
    QHBoxLayout *hbox1 = new QHBoxLayout( vbox, -1 );
    hbox1->addWidget( l );
    hbox1->addWidget( serversList );

    vbox->addWidget( packagesList );

    downloadEnabled = TRUE;
}

void MainWindow :: initProgressWidget()
{
    progressWindow = new QWidget( this );

    QVBoxLayout *layout = new QVBoxLayout( progressWindow, 4, 4 );

    m_status = new QLabel( progressWindow );
    m_status->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
    layout->addWidget( m_status );

    m_progress = new QProgressBar( progressWindow );
    layout->addWidget( m_progress );
}

void MainWindow :: init()
{
#ifdef QWS
    // read download directory from config file
    Config cfg( "aqpkg" );
    cfg.setGroup( "settings" );
    currentlySelectedServer = cfg.readEntry( "selectedServer", "local" );
    //    showJumpTo = cfg.readBoolEntry( "showJumpTo", "true" );

#endif

    stack->raiseWidget( progressWindow );

    mgr = new DataManager();
    connect( mgr, SIGNAL( progressSetSteps( int ) ), this, SLOT( setProgressSteps( int ) ) );
    connect( mgr, SIGNAL( progressSetMessage( const QString & ) ),
             this, SLOT( setProgressMessage( const QString & ) ) );
    connect( mgr, SIGNAL( progressUpdate( int ) ), this, SLOT( updateProgress( int ) ) );
    mgr->loadServers();

    showUninstalledPkgs = false;
    showInstalledPkgs = false;
    showUpgradedPkgs = false;
    categoryFilterEnabled = false;

    updateData();

    stack->raiseWidget( networkPkgWindow );
}

void MainWindow :: setDocument( const QString &doc )
{
    // Remove path from package
    QString package = Utils::getPackageNameFromIpkFilename( doc );

    // First select local server
    for ( int i = 0 ; i < serversList->count() ; ++i )
    {
        if ( serversList->text( i ) == LOCAL_IPKGS )
        {
            serversList->setCurrentItem( i );
            break;
        }
    }
    serverSelected( 0 );

    // Now set the check box of the selected package
    for ( QCheckListItem *item = (QCheckListItem *)packagesList->firstChild();
            item != 0 ;
            item = (QCheckListItem *)item->nextSibling() )
    {
        if ( item->text().startsWith( package ) )
        {
            item->setOn( true );
            break;
        }
    }
}

void MainWindow :: displaySettings()
{
    SettingsImpl *dlg = new SettingsImpl( mgr, this, "Settings", true );
    if ( dlg->showDlg() )
    {
        stack->raiseWidget( progressWindow );
        updateData();
        stack->raiseWidget( networkPkgWindow );
    }
    delete dlg;
}

void MainWindow :: closeEvent( QCloseEvent *e )
{
    // If install dialog is visible, return to main view, otherwise close app
    QWidget *widget = stack->visibleWidget();

    if ( widget != networkPkgWindow && widget != progressWindow )
    {
        if ( widget ) delete widget;
        stack->raiseWidget( networkPkgWindow );
        e->ignore();
    }
    else
    {
        e->accept();
    }
}

void MainWindow :: displayFindBar()
{
    findBar->show();
    findEdit->setFocus();
}

void MainWindow :: displayJumpBar()
{
    jumpBar->show();
}

void MainWindow :: repeatFind()
{
    searchForPackage( findEdit->text() );
}

void MainWindow :: findPackage( const QString &text )
{
    actionFindNext->setEnabled( !text.isEmpty() );
    searchForPackage( text );
}

void MainWindow :: hideFindBar()
{
    findBar->hide();
}

void MainWindow :: hideJumpBar()
{
    jumpBar->hide();
}

void MainWindow :: filterUninstalledPackages()
{
    showUninstalledPkgs = actionUninstalled->isOn();
    if ( showUninstalledPkgs )
    {
        showInstalledPkgs = FALSE;
        showUpgradedPkgs = FALSE;
    }
    serverSelected( -1 );

    actionInstalled->setOn( FALSE );
    actionUpdated->setOn( FALSE );
}

void MainWindow :: filterInstalledPackages()
{
    showInstalledPkgs = actionInstalled->isOn();
    if ( showInstalledPkgs )
    {
        showUninstalledPkgs = FALSE;
        showUpgradedPkgs = FALSE;
    }
    serverSelected( -1 );

    actionUninstalled->setOn( FALSE );
    actionUpdated->setOn( FALSE );
}

void MainWindow :: filterUpgradedPackages()
{
    showUpgradedPkgs = actionUpdated->isOn();
    if ( showUpgradedPkgs )
    {
        showUninstalledPkgs = FALSE;
        showInstalledPkgs = FALSE;
    }
    serverSelected( -1 );

    actionUninstalled->setOn( FALSE );
    actionInstalled->setOn( FALSE );
}

bool MainWindow :: setFilterCategory()
{
    // Get categories;
    CategoryFilterImpl dlg( DataManager::getAvailableCategories(), categoryFilter, this );
    if ( dlg.exec() == QDialog::Accepted )
    {
        categoryFilter = dlg.getSelectedFilter();

        if ( categoryFilter == "" )
            return false;

        categoryFilterEnabled = true;
        serverSelected( -1 );
        actionFilter->setOn( TRUE );
        return true;
    }

    return false;
}

void MainWindow :: filterCategory()
{
    if ( !actionFilter->isOn() )
    {
        filterByCategory( FALSE );
    }
    else
    {
        actionFilter->setOn( filterByCategory( TRUE ) );
    }
}

bool MainWindow :: filterByCategory( bool val )
{
    if ( val )
    {
        if ( categoryFilter == "" )
        {
            if ( !setFilterCategory() )
                return false;
        }

        categoryFilterEnabled = true;
        serverSelected( -1 );
        return true;
    }
    else
    {
        // Turn off filter
        categoryFilterEnabled = false;
        serverSelected( -1 );
        return false;
    }
}

void MainWindow :: raiseMainWidget()
{
    stack->raiseWidget( networkPkgWindow );
}

void MainWindow :: raiseProgressWidget()
{
    stack->raiseWidget( progressWindow );
}

void MainWindow :: enableUpgrade( bool enabled )
{
    actionUpgrade->setEnabled( enabled );
}

void MainWindow :: enableDownload( bool enabled )
{
    if ( enabled )
    {
        actionDownload->setIconSet( iconDownload );
        actionDownload->setText( tr( "Download" ) );
        actionDownload->setWhatsThis( tr( "Click here to download the currently selected package(s)." ) );
    }
    else
    {
        actionDownload->setIconSet( iconRemove );
        actionDownload->setText( tr( "Remove" ) );
        actionDownload->setWhatsThis( tr( "Click here to uninstall the currently selected package(s)." ) );
    }
}

void MainWindow :: setProgressSteps( int numsteps )
{
    m_progress->setTotalSteps( numsteps );
}

void MainWindow :: setProgressMessage( const QString &msg )
{
    m_status->setText( msg );
}

void MainWindow :: updateProgress( int progress )
{
    m_progress->setProgress( progress );
}

void MainWindow :: updateData()
{
    m_progress->setTotalSteps( mgr->getServerList().count() );

    serversList->clear();
    packagesList->clear();

    int activeItem = -1;
    int i = 0;
    QString serverName;

    QListIterator<Server> it( mgr->getServerList() );
    Server *server;

    for ( ; it.current(); ++it, ++i )
    {
        server = it.current();
        serverName = server->getServerName();
        m_status->setText( tr( "Building server list:\n\t%1" ).arg( serverName ) );
        m_progress->setProgress( i );
        qApp->processEvents();

        if ( !server->isServerActive() )
        {
            i--;
            continue;
        }

        serversList->insertItem( serverName );
        if ( serverName == currentlySelectedServer )
            activeItem = i;
    }

    // set selected server to be active server
    if ( activeItem != -1 )
        serversList->setCurrentItem( activeItem );
    serverSelected( 0, FALSE );
}

void MainWindow :: serverSelected( int index )
{
    serverSelected( index, TRUE );
}

void MainWindow :: serverSelected( int, bool raiseProgress )
{
    QPixmap nullIcon( installedIcon.size() );
    nullIcon.fill( colorGroup().base() );

    // display packages
    QString serverName = serversList->currentText();
    currentlySelectedServer = serverName;

    Server *s = mgr->getServer( serverName );

    QList<Package> &list = s->getPackageList();
    QListIterator<Package> it( list );

    // Display progress widget while loading list
    bool doProgress = ( list.count() > 200 );
    if ( doProgress )
    {
        if ( raiseProgress )
        {
            stack->raiseWidget( progressWindow );
        }
        m_progress->setTotalSteps( list.count() );
        m_status->setText( tr( "Building package list for:\n\t%1" ).arg( serverName ) );
    }

    packagesList->clear();

#ifdef QWS
    // read download directory from config file
    Config cfg( "aqpkg" );
    cfg.setGroup( "settings" );
    cfg.writeEntry( "selectedServer", currentlySelectedServer );
#endif

    int i = 0;
    Package *package;
    for ( ; it.current(); ++it )
    {
        // Update progress after every 100th package (arbitrary value, seems to give good balance)
        i++;
        if ( ( i % 100 ) == 0 )
        {
            if ( doProgress )
            {
                m_progress->setProgress( i );
            }
            qApp->processEvents();
        }

        QString text = "";

        package = it.current();

        // Apply show only uninstalled packages filter
        if ( showUninstalledPkgs && package->isInstalled() )
            continue;

        // Apply show only installed packages filter
        if ( showInstalledPkgs && !package->isInstalled() )
            continue;

        // Apply show only new installed packages filter
        if ( showUpgradedPkgs )
        {
            if ( !package->isInstalled() || !package->getNewVersionAvailable() )
                continue;
        }

        // Apply the section filter
        if ( categoryFilterEnabled && categoryFilter != "" )
        {
            if ( package->getSection() == "" || categoryFilter.find( package->getSection().lower() ) == -1 )
                continue;
        }

        // If the local server, only display installed packages
        if ( serverName == LOCAL_SERVER && !package->isInstalled() )
            continue;


        QCheckListItem *item = new QCheckListItem( packagesList, package->getPackageName(),
                               QCheckListItem::CheckBox );

        if ( package->isInstalled() )
        {
            // If a different version of package is available, show update available icon
            // Otherwise, show installed icon
            if ( package->getNewVersionAvailable())
            {

                item->setPixmap( 0, updatedIcon );
            }
            else
            {
                item->setPixmap( 0, installedIcon );
            }
        }
        else
        {
            item->setPixmap( 0, nullIcon );
        }

        packagesList->insertItem( item );
    }

    // If the local server or the local ipkgs server disable the download button
    if ( serverName == LOCAL_SERVER )
    {
        downloadEnabled = TRUE;
        actionUpgrade->setEnabled( FALSE );
    }
    else if ( serverName == LOCAL_IPKGS )
    {
        downloadEnabled = FALSE;
        actionUpgrade->setEnabled( FALSE );
    }
    else
    {
        downloadEnabled = TRUE;
        actionUpgrade->setEnabled( TRUE );
    }
    enableDownload( downloadEnabled );

    // Display this widget once everything is done
    if ( doProgress && raiseProgress )
    {
        stack->raiseWidget( networkPkgWindow );
    }
}

void MainWindow :: searchForPackage( const QString &text )
{
    if ( !text.isEmpty() )
    {
        // look through package list for text startng at current position
        QCheckListItem *start = (QCheckListItem *)packagesList->currentItem();

        if ( start == 0 )
            start = (QCheckListItem *)packagesList->firstChild();

        for ( QCheckListItem *item = start; item != 0 ;
                item = (QCheckListItem *)item->nextSibling() )
        {
            if ( item->text().lower().find( text ) != -1 )
            {
                packagesList->ensureItemVisible( item );
                packagesList->setCurrentItem( item );
                break;
            }
        }
    }
}

void MainWindow :: updateServer()
{
    QString serverName = serversList->currentText();

    // Update the current server
    // Display dialog

    // Disable buttons to stop silly people clicking lots on them :)

    // First, write out ipkg_conf file so that ipkg can use it
    mgr->writeOutIpkgConf();

    Ipkg *ipkg = new Ipkg;
    ipkg->setOption( "update" );

    InstallDlgImpl *dlg = new InstallDlgImpl( ipkg, tr( "Refreshing server package lists" ),
                          tr( "Update lists" ) );
    connect( dlg, SIGNAL( reloadData( InstallDlgImpl * ) ), this, SLOT( reloadData( InstallDlgImpl * ) ) );
    reloadDocuments = FALSE;
    stack->addWidget( dlg, 3 );
    stack->raiseWidget( dlg );

    //  delete progDlg;
}

void MainWindow :: upgradePackages()
{
    // We're gonna do an upgrade of all packages
    // First warn user that this isn't recommended
    // TODO - ODevice????
    QString text = tr( "WARNING: Upgrading while\nOpie/Qtopia is running\nis NOT recommended!\n\nAre you sure?\n" );
    QMessageBox warn( tr( "Warning" ), text, QMessageBox::Warning,
                      QMessageBox::Yes,
                      QMessageBox::No | QMessageBox::Escape | QMessageBox::Default ,
                      0, this );
    warn.adjustSize();

    if ( warn.exec() == QMessageBox::Yes )
    {
        // First, write out ipkg_conf file so that ipkg can use it
        mgr->writeOutIpkgConf();

        // Now run upgrade
        Ipkg *ipkg = new Ipkg;
        ipkg->setOption( "upgrade" );

        InstallDlgImpl *dlg = new InstallDlgImpl( ipkg, tr( "Upgrading installed packages" ),
                              tr ( "Upgrade" ) );
        connect( dlg, SIGNAL( reloadData( InstallDlgImpl * ) ), this, SLOT( reloadData( InstallDlgImpl * ) ) );
        reloadDocuments = TRUE;
        stack->addWidget( dlg, 3 );
        stack->raiseWidget( dlg );
    }
}

void MainWindow :: downloadPackage()
{
    bool doUpdate = true;
    if ( downloadEnabled )
    {
        // See if any packages are selected
        bool found = false;
        if ( serversList->currentText() != LOCAL_SERVER )
        {
            for ( QCheckListItem *item = (QCheckListItem *)packagesList->firstChild();
                    item != 0 && !found;
                    item = (QCheckListItem *)item->nextSibling() )
            {
                if ( item->isOn() )
                    found = true;
            }
        }

        // If user selected some packages then download the and store the locally
        // otherwise, display dialog asking user what package to download from an http server
        // and whether to install it
        if ( found )
            downloadSelectedPackages();
        else
            downloadRemotePackage();

    }
    else
    {
        doUpdate = false;
        for ( QCheckListItem *item = (QCheckListItem *)packagesList->firstChild();
                item != 0 ;
                item = (QCheckListItem *)item->nextSibling() )
        {
            if ( item->isOn() )
            {
                QString name = item->text();
                int pos = name.find( "*" );
                name.truncate( pos );

                // if (there is a (installed), remove it
                pos = name.find( "(installed)" );
                if ( pos > 0 )
                    name.truncate( pos - 1 );

                Package *p = mgr->getServer( serversList->currentText() )->getPackage( name );

                QString msgtext;
                msgtext = tr( "Are you sure you wish to delete\n%1?" ).arg( (const char *)p->getPackageName() );
                if ( QMessageBox::information( this, tr( "Are you sure?" ),
                                               msgtext, tr( "No" ), tr( "Yes" ) ) == 1 )
                {
                    doUpdate = true;
                    QFile f( p->getFilename() );
                    f.remove();
                }
            }
        }
    }

    if ( doUpdate )
    {
        reloadData( 0x0 );
    }
}

void MainWindow :: downloadSelectedPackages()
{
    // First, write out ipkg_conf file so that ipkg can use it
    mgr->writeOutIpkgConf();

    // Display dialog to user asking where to download the files to
    bool ok = FALSE;
    QString dir = "";
#ifdef QWS
    // read download directory from config file
    Config cfg( "aqpkg" );
    cfg.setGroup( "settings" );
    dir = cfg.readEntry( "downloadDir", "/home/root/Documents/application/ipkg" );
#endif

    QString text = InputDialog::getText( tr( "Download to where" ), tr( "Enter path to download to" ), dir, &ok, this );
    if ( ok && !text.isEmpty() )
        dir = text;   // user entered something and pressed ok
    else
        return; // user entered nothing or pressed cancel

#ifdef QWS
    // Store download directory in config file
    cfg.writeEntry( "downloadDir", dir );
#endif

    // Get starting directory
    char initDir[PATH_MAX];
    getcwd( initDir, PATH_MAX );

    // Download each package
    Ipkg ipkg;
    connect( &ipkg, SIGNAL(outputText(const QString &)), this, SLOT(displayText(const QString &)));

    ipkg.setOption( "download" );
    ipkg.setRuntimeDirectory( dir );
    for ( QCheckListItem *item = (QCheckListItem *)packagesList->firstChild();
            item != 0 ;
            item = (QCheckListItem *)item->nextSibling() )
    {
        if ( item->isOn() )
        {
            ipkg.setPackage( item->text() );
            ipkg.runIpkg( );
        }
    }
}

void MainWindow :: downloadRemotePackage()
{
    // Display dialog
    bool ok;
    QString package = InputDialog::getText( tr( "Install Remote Package" ), tr( "Enter package location" ), "http://", &ok, this );
    if ( !ok || package.isEmpty() )
        return;
    //    DownloadRemoteDlgImpl dlg( this, "Install", true );
    //    if ( dlg.exec() == QDialog::Rejected )
    //        return;

    // grab details from dialog
    //    QString package = dlg.getPackageLocation();

    InstallData *item = new InstallData();
    item->option = "I";
    item->packageName = package;
    QList<InstallData> workingPackages;
    workingPackages.setAutoDelete( TRUE );
    workingPackages.append( item );

    InstallDlgImpl *dlg = new InstallDlgImpl( workingPackages, mgr, tr( "Download" ) );
    connect( dlg, SIGNAL( reloadData( InstallDlgImpl * ) ), this, SLOT( reloadData( InstallDlgImpl * ) ) );
    reloadDocuments = TRUE;
    stack->addWidget( dlg, 3 );
    stack->raiseWidget( dlg );
}


void MainWindow :: applyChanges()
{
    stickyOption = "";

    // First, write out ipkg_conf file so that ipkg can use it
    mgr->writeOutIpkgConf();

    // Now for each selected item
    // deal with it

    QList<InstallData> workingPackages;
    workingPackages.setAutoDelete( TRUE );
    for ( QCheckListItem *item = (QCheckListItem *)packagesList->firstChild();
            item != 0 ;
            item = (QCheckListItem *)item->nextSibling() )
    {
        if ( item->isOn() )
        {
            InstallData *instdata = dealWithItem( item );
            if ( instdata )
                workingPackages.append( instdata );
            else
                return;
        }
    }

    if ( workingPackages.count() == 0 )
    {
        // Nothing to do
        QMessageBox::information( this, tr( "Nothing to do" ),
                                  tr( "No packages selected" ), tr( "OK" ) );

        return;
    }

    // do the stuff
    InstallDlgImpl *dlg = new InstallDlgImpl( workingPackages, mgr, tr( "Apply changes" ) );
    connect( dlg, SIGNAL( reloadData( InstallDlgImpl * ) ), this, SLOT( reloadData( InstallDlgImpl * ) ) );
    reloadDocuments = TRUE;
    stack->addWidget( dlg, 3 );
    stack->raiseWidget( dlg );
}

// decide what to do - either remove, upgrade or install
// Current rules:
//    If not installed - install
//    If installed and different version available - upgrade
//    If installed and version up to date - remove
InstallData *MainWindow :: dealWithItem( QCheckListItem *item )
{
    QString name = item->text();

    // Get package
    Server *s = mgr->getServer( serversList->currentText() );
    Package *p = s->getPackage( name );

    // If the package has a filename then it is a local file
    if ( p->isPackageStoredLocally() )
        name = p->getFilename();

    QString option;
    QString dest = "root";
    if ( !p->isInstalled() )
    {
        InstallData *newitem = new InstallData();
        newitem->option = "I";
        newitem->packageName = name;
        return newitem;
    }
    else
    {
        InstallData *newitem = new InstallData();
        newitem->option = "D";
        // If local file, remove using package name, not filename
        if ( p->isPackageStoredLocally() )
            name = item->text();

        if ( !p->isPackageStoredLocally() )
            newitem->packageName = p->getInstalledPackageName();
        else
            newitem->packageName = name;

        if ( p->getInstalledTo() )
        {
            newitem->destination = p->getInstalledTo();
        }
        else
        {
            newitem->destination = p->getLocalPackage()->getInstalledTo();
        }

        // Now see if version is newer or not
        int val = compareVersions( p->getInstalledVersion(), p->getVersion() );

        // If the version requested is older and user selected a local ipk file, then reinstall the file
        if ( p->isPackageStoredLocally() && val == -1 )
            val = 0;

        if ( val == -2 )
        {
            // Error - should handle
        }
        else if ( val == -1 )
        {
            // Version available is older - remove only
            newitem->option = "D";
            // If local file, remove using package name, not filename
            if ( p->isPackageStoredLocally() )
                name = item->text();
        }
        else
        {
            QString caption;
            QString text;
            QString secondButton;
            QString secondOption;
            if ( val == 0 )
            {
                // Version available is the same - option to remove or reinstall
                caption = tr( "Do you wish to remove or reinstall\n%1?" );
                text = tr( "Remove or ReInstall" );
                secondButton = tr( "ReInstall" );
                secondOption = "R"; // Internal action code, do not translate
            }
            else if ( val == 1 )
            {
                // Version available is newer - option to remove or upgrade
                caption = tr( "Do you wish to remove or upgrade\n%1?" );
                text = tr( "Remove or Upgrade" );
                secondButton = tr( "Upgrade" );
                secondOption = "U"; // Internal action code, do not translate
            }

            // Sticky option not implemented yet, but will eventually allow
            // the user to say something like 'remove all'
            if ( stickyOption == "" )
            {
                QString msgtext;
                msgtext = caption.arg( ( const char * )name );
                QuestionDlg dlg( text, msgtext, secondButton );
                switch( dlg.exec() )
                {
                case 0: // Cancel
                    delete newitem;
                    return 0x0;
                    break;
                case 1: // Remove
                    newitem->option = "D";
                    // If local file, remove using package name, not filename
                    if ( p->isPackageStoredLocally() )
                        name = item->text();
                    break;
                case 2: // Reinstall or Upgrade
                    newitem->option = secondOption;
                    break;
                }
            }
            else
            {
                //                newitem->option = stickyOption;
            }
        }


        // Check if we are reinstalling the same version
        if ( newitem->option != "R" )
            newitem->recreateLinks = true;
        else
            newitem->recreateLinks = false;

        // User hit cancel (on dlg - assume remove)
        return newitem;
    }
}

void MainWindow :: reloadData( InstallDlgImpl *dlg )
{
    stack->raiseWidget( progressWindow );

    if ( dlg )
    {
        dlg->close();
        delete dlg;
    }

    mgr->reloadServerData();
    serverSelected( -1, FALSE );

#ifdef QWS
    if ( reloadDocuments )
    {
        m_status->setText( tr( "Updating Launcher..." ) );

        // Finally let the main system update itself
        QCopEnvelope e("QPE/System", "linkChanged(QString)");
        QString lf = QString::null;
        e << lf;
    }
#endif

    stack->raiseWidget( networkPkgWindow );
}

void MainWindow :: letterPushed( QString t )
{
    QCheckListItem *top = (QCheckListItem *)packagesList->firstChild();
    QCheckListItem *start = (QCheckListItem *)packagesList->currentItem();
    if ( packagesList->firstChild() == 0 )
        return;

    QCheckListItem *item;
    if ( start == 0 )
    {
        item = (QCheckListItem *)packagesList->firstChild();
        start = top;
    }
    else
        item = (QCheckListItem *)start->nextSibling();

    if ( item == 0 )
        item = (QCheckListItem *)packagesList->firstChild();
    do
    {
        if ( item->text().lower().startsWith( t.lower() ) )
        {
            packagesList->setSelected( item, true );
            packagesList->ensureItemVisible( item );
            break;
        }

        item = (QCheckListItem *)item->nextSibling();
        if ( !item )
            item = (QCheckListItem *)packagesList->firstChild();
    }
    while ( item != start);
}

void MainWindow :: slotDisplayPackage( QListViewItem *item )
{
    QString itemstr( ((QCheckListItem*)item)->text() );
    PackageWindow *p = new PackageWindow( mgr->getServer( serversList->currentText() )->getPackage( itemstr ) );
    QPEApplication::showWidget( p );
}

QuestionDlg::QuestionDlg( const QString &caption, const QString &text, const QString &secondbtn )
        : QWidget( 0x0, 0x0, WType_Modal | WType_TopLevel | WStyle_Dialog )
{
    setCaption( caption );
    resize( 175, 100 );

    QGridLayout *layout = new QGridLayout( this );

    QLabel *l = new QLabel( text, this );
    l->setAlignment( AlignCenter | WordBreak );
    layout->addMultiCellWidget( l, 0, 0, 0, 1 );

    btn1 = new QPushButton( tr( "Remove" ), this );
    connect( btn1, SIGNAL(clicked()), this, SLOT(slotButtonPressed()) );
    layout->addWidget( btn1, 1, 0 );

    btn2 = new QPushButton( secondbtn, this );
    connect( btn2, SIGNAL(clicked()), this, SLOT(slotButtonPressed()) );
    layout->addWidget( btn2, 1, 1 );

    executing = FALSE;
}

int QuestionDlg::exec()
{
    show();

    if ( !executing )
    {
        executing = TRUE;
        qApp->enter_loop();
    }

    return buttonpressed;
}

void QuestionDlg::slotButtonPressed()
{
    if ( sender() == btn1 )
        buttonpressed = 1;
    else if ( sender() == btn2 )
        buttonpressed = 2;
    else
        buttonpressed = 0;

    qApp->exit_loop();
}
