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

#include <stdio.h>

#include <opie2/ofiledialog.h>

#ifdef QWS
#include <qpe/config.h>
#include <qpe/fileselector.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/storage.h>
#endif

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qfileinfo.h>
#include <qgroupbox.h>
#include <qmultilineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>

#include "datamgr.h"
#include "destination.h"
#include "instoptionsimpl.h"
#include "installdlgimpl.h"
#include "ipkg.h"
#include "utils.h"
#include "global.h"

enum {
    MAXLINES = 100,
};

InstallDlgImpl::InstallDlgImpl( const QList<InstallData> &packageList, DataManager *dataManager, const char *title )
    : QWidget( 0, 0, 0 )
{
    setCaption( title );
    init( TRUE );

    pIpkg = 0;
    upgradePackages = false;
    dataMgr = dataManager;

    QString defaultDest = "root";
#ifdef QWS
    Config cfg( "aqpkg" );
    cfg.setGroup( "settings" );
    defaultDest = cfg.readEntry( "dest", "root" );

    // Grab flags - Turn MAKE_LINKS on by default (if no flags found)
    flags = cfg.readNumEntry( "installFlags", 0 );
    infoLevel = cfg.readNumEntry( "infoLevel", 1 );
#else
    flags = 0;
#endif

    // Output text is read only
    output->setReadOnly( true );
//    QFont f( "helvetica" );
//    f.setPointSize( 10 );
//    output->setFont( f );


    // setup destination data
    int defIndex = 0;
    int i;
    QListIterator<Destination> dit( dataMgr->getDestinationList() );
    for ( i = 0; dit.current(); ++dit, ++i )
    {
        destination->insertItem( dit.current()->getDestinationName() );
        if ( dit.current()->getDestinationName() == defaultDest )
            defIndex = i;
    }

    destination->setCurrentItem( defIndex );

    QListIterator<InstallData> it( packageList );
    // setup package data
    QString remove = tr( "Remove\n" );
    QString install = tr( "Install\n" );
    QString upgrade = tr( "Upgrade\n" );
    for ( ; it.current(); ++it )
    {
        InstallData *item = it.current();
        InstallData *newitem = new InstallData();

        newitem->option = item->option;
        newitem->packageName = item->packageName;
        newitem->destination = item->destination;
        newitem->recreateLinks = item->recreateLinks;
        packages.append( newitem );

        if ( item->option == "I" )
        {
            install.append( QString( "   %1\n" ).arg( item->packageName ) );
        }
        else if ( item->option == "D" )
        {
            remove.append( QString( "   %1\n" ).arg( item->packageName ) );
        }
        else if ( item->option == "U" || item->option == "R" )
        {
            QString type;
            if ( item->option == "R" )
                type = tr( "(ReInstall)" );
            else
                type = tr( "(Upgrade)" );
            upgrade.append( QString( "   %1 %2\n" ).arg( item->packageName ).arg( type ) );
        }
    }

    output->setText( QString( "%1\n%2\n%3\n" ).arg( remove ).arg( install ).arg( upgrade ) );

    displayAvailableSpace( destination->currentText() );
}

InstallDlgImpl::InstallDlgImpl( Ipkg *ipkg, QString initialText, const char *title )
    : QWidget( 0, 0, 0 )
{
    setCaption( title );
    init( FALSE );
    pIpkg = ipkg;
    output->setText( initialText );
}


InstallDlgImpl::~InstallDlgImpl()
{
    if ( pIpkg )
        delete pIpkg;
}

void InstallDlgImpl :: init( bool displayextrainfo )
{
    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 4 );
    layout->setMargin( 4 );

    if ( displayextrainfo )
    {
        QLabel *label = new QLabel( tr( "Destination" ), this );
        layout->addWidget( label, 0, 0 );
        destination = new QComboBox( FALSE, this );
        layout->addWidget( destination, 0, 1 );
        connect( destination, SIGNAL( highlighted( const QString & ) ),
                this, SLOT( displayAvailableSpace( const QString & ) ) );

        QLabel *label2 = new QLabel( tr( "Space Avail" ), this );
        layout->addWidget( label2, 1, 0 );
        txtAvailableSpace = new QLabel( "", this );
        layout->addWidget( txtAvailableSpace, 1, 1 );
    }
    else
    {
        destination = 0x0;
        txtAvailableSpace = 0x0;
    }

    QGroupBox *GroupBox2 = new QGroupBox( 0, Qt::Vertical, tr( "Output" ), this );
    GroupBox2->layout()->setSpacing( 0 );
    GroupBox2->layout()->setMargin( 4 );

    QVBoxLayout *GroupBox2Layout = new QVBoxLayout( GroupBox2->layout() );
    output = new QMultiLineEdit( GroupBox2 );
    GroupBox2Layout->addWidget( output );
    layout->addMultiCellWidget( GroupBox2, 2, 2, 0, 1 );

    btnInstall = new QPushButton( Resource::loadPixmap( "aqpkg/apply" ), tr( "Start" ), this );
    layout->addWidget( btnInstall, 3, 0 );
    connect( btnInstall, SIGNAL( clicked() ), this, SLOT( installSelected() ) );

    btnOptions = new QPushButton( Resource::loadPixmap( "SettingsIcon" ), tr( "Options" ), this );
    layout->addWidget( btnOptions, 3, 1 );
    connect( btnOptions, SIGNAL( clicked() ), this, SLOT( optionsSelected() ) );
}

void InstallDlgImpl :: optionsSelected()
{
    if ( btnOptions->text() == tr( "Options" ) )
    {
        InstallOptionsDlgImpl opt( flags, infoLevel, this, "Option", true );
        if ( opt.exec() == QDialog::Accepted )
        {
            // set options selected from dialog
            flags = opt.getFlags();
            infoLevel = opt.getInfoLevel();

#ifdef QWS
            Config cfg( "aqpkg" );
            cfg.setGroup( "settings" );
            cfg.writeEntry( "installFlags", flags );
            cfg.writeEntry( "infoLevel", infoLevel );
#endif
        }
    }
    else // Save output
    {
        QMap<QString, QStringList> map;
        map.insert( tr( "All" ), QStringList() );
        QStringList text;
        text << "text/*";
        map.insert(tr( "Text" ), text );
        text << "*";
        map.insert( tr( "All" ), text );

        QString filename = Opie::OFileDialog::getSaveFileName( 2, "/", "ipkg-output", map );
        if( !filename.isEmpty() )
        {
            QString currentFileName = QFileInfo( filename ).fileName();
            DocLnk doc;
            doc.setType( "text/plain" );
            doc.setFile( filename );
            doc.setName( currentFileName );
            FileManager fm;
            fm.saveFile( doc, output->text() );
        }
    }
}

void InstallDlgImpl :: installSelected()
{
    if ( btnInstall->text() == tr( "Abort" ) )
    {
        if ( pIpkg )
        {
            displayText( tr( "\n**** User Clicked ABORT ***" ) );
            pIpkg->abort();
            displayText( tr( "**** Process Aborted ****" ) );
        }

        btnInstall->setText( tr( "Close" ) );
        btnInstall->setIconSet( Resource::loadPixmap( "enter" ) );
        return;
    }
    else if ( btnInstall->text() == tr( "Close" ) )
    {
        emit reloadData( this );
        return;
    }

    // Disable buttons
    btnOptions->setEnabled( false );
//    btnInstall->setEnabled( false );

    btnInstall->setText( tr( "Abort" ) );
    btnInstall->setIconSet( Resource::loadPixmap( "close" ) );

    if ( pIpkg )
    {
        output->setText( "" );
        connect( pIpkg, SIGNAL(outputText(const QString &)), this, SLOT(displayText(const QString &)));
        connect( pIpkg, SIGNAL(ipkgFinished()), this, SLOT(ipkgFinished()));
        pIpkg->runIpkg();
    }
    else
    {
        output->setText( "" );
        Destination *d = dataMgr->getDestination( destination->currentText() );
        QString dest = d->getDestinationName();
        QString destDir = d->getDestinationPath();
        int instFlags = flags;
        if ( d->linkToRoot() )
            instFlags |= MAKE_LINKS;

#ifdef QWS
        // Save settings
        Config cfg( "aqpkg" );
        cfg.setGroup( "settings" );
        cfg.writeEntry( "dest", dest );
#endif

        pIpkg = new Ipkg;
        connect( pIpkg, SIGNAL(outputText(const QString &)), this, SLOT(displayText(const QString &)));
        connect( pIpkg, SIGNAL(ipkgFinished()), this, SLOT(ipkgFinished()));

        firstPackage = TRUE;
        ipkgFinished();

        // First run through the remove list, then the install list then the upgrade list
/*
        pIpkg->setOption( "remove" );
        QListIterator<InstallData> it( removeList );
        InstallData *idata;
        for ( ; it.current(); ++it )
        {
            idata = it.current();
            pIpkg->setDestination( idata->destination->getDestinationName() );
            pIpkg->setDestinationDir( idata->destination->getDestinationPath() );
            pIpkg->setPackage( idata->packageName );

            int tmpFlags = flags;
            if ( idata->destination->linkToRoot() )
                tmpFlags |= MAKE_LINKS;

            pIpkg->setFlags( tmpFlags, infoLevel );
            pIpkg->runIpkg();
        }

        pIpkg->setOption( "install" );
        pIpkg->setDestination( dest );
        pIpkg->setDestinationDir( destDir );
        pIpkg->setFlags( instFlags, infoLevel );
        QListIterator<InstallData> it2( installList );
        for ( ; it2.current(); ++it2 )
        {
            pIpkg->setPackage( it2.current()->packageName );
            pIpkg->runIpkg();
        }

        flags |= FORCE_REINSTALL;
        QListIterator<InstallData> it3( updateList );
        for ( ; it3.current() ; ++it3 )
        {
            idata = it3.current();
            if ( idata->option == "R" )
                pIpkg->setOption( "reinstall" );
            else
                pIpkg->setOption( "upgrade" );
            pIpkg->setDestination( idata->destination->getDestinationName() );
            pIpkg->setDestinationDir( idata->destination->getDestinationPath() );
            pIpkg->setPackage( idata->packageName );

            int tmpFlags = flags;
            if ( idata->destination->linkToRoot() && idata->recreateLinks )
                tmpFlags |= MAKE_LINKS;
            pIpkg->setFlags( tmpFlags, infoLevel );
            pIpkg->runIpkg();
        }

        delete pIpkg;
        pIpkg = 0;
*/
    }
}


void InstallDlgImpl :: displayText(const QString &text )
{
    QString newtext = QString( "%1\n%2" ).arg( output->text() ).arg( text );

    /* Set a max line count for the QMultiLineEdit, as users have reported
     * performance issues when line count gets extreme.
     */
    if(output->numLines() >= MAXLINES)
        output->removeLine(0);
    output->setText( newtext );
    output->setCursorPosition( output->numLines(), 0 );
}


void InstallDlgImpl :: displayAvailableSpace( const QString &text )
{
    Destination *d = dataMgr->getDestination( text );
    QString destDir = d->getDestinationPath();

    long blockSize = 0;
    long totalBlocks = 0;
    long availBlocks = 0;
    QString space;
    if ( Utils::getStorageSpace( (const char *)destDir, &blockSize, &totalBlocks, &availBlocks ) )
    {
        long mult = blockSize / 1024;
        long div = 1024 / blockSize;

        if ( !mult ) mult = 1;
        if ( !div ) div = 1;
        long avail = availBlocks * mult / div;

        space = tr( "%1 Kb" ).arg( avail );
    }
    else
        space = tr( "Unknown" );

    if ( txtAvailableSpace )
        txtAvailableSpace->setText( space );
}

void InstallDlgImpl :: ipkgFinished()
{
    InstallData *item;
    if ( firstPackage )
        item = packages.first();
    else
    {
        // Create symlinks if necessary before moving on to next package
        pIpkg->createSymLinks();

        item = packages.next();
    }

    firstPackage = FALSE;
    if ( item )
    {
        pIpkg->setPackage( item->packageName );
        int tmpFlags = flags;

        if ( item->option == "I" )
        {
            pIpkg->setOption( "install" );
            Destination *d = dataMgr->getDestination( destination->currentText() );
            pIpkg->setDestination( d->getDestinationName() );
            pIpkg->setDestinationDir( d->getDestinationPath() );

            if ( d->linkToRoot() )
                tmpFlags |= MAKE_LINKS;
        }
        else if ( item->option == "D" )
        {
            pIpkg->setOption( "remove" );
            pIpkg->setDestination( item->destination->getDestinationName() );
            pIpkg->setDestinationDir( item->destination->getDestinationPath() );

            if ( item->destination->linkToRoot() )
                tmpFlags |= MAKE_LINKS;
        }
        else
        {
            if ( item->option == "R" )
                pIpkg->setOption( "reinstall" );
            else
                pIpkg->setOption( "upgrade" );

            pIpkg->setDestination( item->destination->getDestinationName() );
            pIpkg->setDestinationDir( item->destination->getDestinationPath() );
            pIpkg->setPackage( item->packageName );

            tmpFlags |= FORCE_REINSTALL;
            if ( item->destination->linkToRoot() && item->recreateLinks )
                tmpFlags |= MAKE_LINKS;
        }
        pIpkg->setFlags( tmpFlags, infoLevel );
        pIpkg->runIpkg();
    }
    else
    {
        btnOptions->setEnabled( true );
        btnInstall->setText( tr( "Close" ) );
        btnInstall->setIconSet( Resource::loadPixmap( "enter" ) );

        btnOptions->setText( tr( "Save output" ) );
        btnOptions->setIconSet( Resource::loadPixmap( "save" ) );

        if ( destination && destination->currentText() != 0 && destination->currentText() != "" )
            displayAvailableSpace( destination->currentText() );
    }
}
