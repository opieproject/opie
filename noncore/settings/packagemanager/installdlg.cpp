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

#include "installdlg.h"

#include <opie2/ofiledialog.h>

#include <qpe/fileselector.h>
#include <qpe/resource.h>
#include <qpe/storage.h>

#include <qapplication.h>
#include <qcombobox.h>
#include <qfileinfo.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmap.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>

#include <sys/vfs.h>

#include "opackagemanager.h"

InstallDlg::InstallDlg( QWidget *parent, OPackageManager *pm, const QString &caption,
                        OPackage::Command command1, const QStringList &packages1,
                        OPackage::Command command2, const QStringList &packages2,
                        OPackage::Command command3, const QStringList &packages3 )
    : QWidget( 0l )
    , m_packman( pm )
    , m_installFound( false )
    , m_numCommands( 0 )
    , m_currCommand( 0 )
    , m_destItem( 0l )
{
    // Save command/package list information
    if ( command1 != OPackage::NotDefined )
    {
        m_command[ m_numCommands ] = command1;
        m_packages[ m_numCommands ] = packages1;
        ++m_numCommands;

        if ( command1 == OPackage::Install )
            m_installFound = true;
    }
    if ( command2 != OPackage::NotDefined )
    {
        m_command[ m_numCommands ] = command2;
        m_packages[ m_numCommands ] = packages2;
        ++m_numCommands;

        if ( command2 == OPackage::Install )
            m_installFound = true;
    }
    if ( command3 != OPackage::NotDefined )
    {
        m_command[ m_numCommands ] = command3;
        m_packages[ m_numCommands ] = packages3;
        ++m_numCommands;

        if ( command3 == OPackage::Install )
            m_installFound = true;
    }

    // Initialize UI
    if ( parent )
        parent->setCaption( caption );

    QGridLayout *layout = new QGridLayout( this, 4, 2, 2, 4 );

    if ( m_installFound )
    {
        QLabel *label = new QLabel( tr( "Destination" ), this );
        layout->addWidget( label, 0, 0 );
        m_destination = new QComboBox( this );
        m_destination->insertStringList( m_packman->destinations() );
        layout->addWidget( m_destination, 0, 1 );
        connect( m_destination, SIGNAL(highlighted(const QString&)),
                this, SLOT(slotDisplayAvailSpace(const QString&)) );

        label = new QLabel( tr( "Space Avail" ), this );
        layout->addWidget( label, 1, 0 );
        m_availSpace = new QLabel( this );
        layout->addWidget( m_availSpace, 1, 1 );

        // TODO - select correct destination
        slotDisplayAvailSpace( m_destination->currentText() );
    }
    else
    {
        m_destination = 0l;
        m_availSpace = 0l;
    }

    QGroupBox *groupBox = new QGroupBox( 0, Qt::Vertical, tr( "Output" ), this );
    groupBox->layout()->setSpacing( 0 );
    groupBox->layout()->setMargin( 4 );

    QVBoxLayout *groupBoxLayout = new QVBoxLayout( groupBox->layout() );
    m_output = new QMultiLineEdit( groupBox );
    m_output->setReadOnly( true );
    groupBoxLayout->addWidget( m_output );
    layout->addMultiCellWidget( groupBox, 2, 2, 0, 1 );

    QPixmap pic;
    pic.convertFromImage( Resource::loadImage( "packagemanager/apply" ).smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() ) );
    m_btnStart = new QPushButton( pic, tr( "Start" ), this );
    m_btnStart->setMinimumHeight( AppLnk::smallIconSize() );
    layout->addWidget( m_btnStart, 3, 0 );
    connect( m_btnStart, SIGNAL(clicked()), this, SLOT(slotBtnStart()) );

    pic.convertFromImage( Resource::loadImage( "SettingsIcon" ).smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() ) );
    m_btnOptions = new QPushButton( pic, tr( "Options" ), this );
    m_btnOptions->setMinimumHeight( AppLnk::smallIconSize() );
    layout->addWidget( m_btnOptions, 3, 1 );
    connect( m_btnOptions, SIGNAL( clicked() ), this, SLOT(slotBtnOptions()) );

    // Display packages being acted upon in output widget
    for( int i = 0; i < m_numCommands; i++ )
    {
        if ( !m_packages[ i ].isEmpty() )
        {
            QString lineStr = tr( "Packages to " );

            switch( m_command[ i ] )
            {
                case OPackage::Install : lineStr.append( tr( "install" ) );
                    break;
                case OPackage::Remove : lineStr.append( tr( "remove" ) );
                    break;
                case OPackage::Upgrade : lineStr.append( tr( "upgrade" ) );
                    break;
                case OPackage::Download : lineStr.append( tr( "download" ) );
                    break;
                default :
                    break;
            };
            lineStr.append( ":\n" );

            QStringList tmpPackage = m_packages[ i ];
            for ( QStringList::Iterator it = tmpPackage.begin(); it != tmpPackage.end(); ++it )
            {
                lineStr.append( QString( "\t%1\n" ).arg( ( *it ) ) );
            }

            m_output->append( lineStr );
        }
    }

    m_output->append( tr( "Press the start button to begin.\n" ) );
    m_output->setCursorPosition( m_output->numLines(), 0 );

}

void InstallDlg::slotDisplayAvailSpace( const QString &destination )
{
    // If available space is not displayed, exit
    if ( !m_availSpace )
        return;

    QString space = tr( "Unknown" );

    // Get destination
    if ( !destination.isNull() )
        m_destItem = m_packman->findConfItem( OConfItem::Destination, destination );

    if ( m_destItem )
    {
        // Calculate available space
        struct statfs fs;
        if ( !statfs( m_destItem->value(), &fs ) )
        {
            long mult = fs.f_bsize / 1024;
            long div = 1024 / fs.f_bsize;

            if ( !mult ) mult = 1;
            if ( !div ) div = 1;
            long avail = fs.f_bavail * mult / div;

            space = tr( "%1 Kb" ).arg( avail );
        }
    }

    // Display available space
    m_availSpace->setText( space );
}

void InstallDlg::slotBtnStart()
{
    QString btnText = m_btnStart->text();
    if ( btnText == tr( "Abort" ) )
    {
        // Prevent unexecuted commands from executing
        m_currCommand = 999;

        // Allow user to close dialog
        m_btnStart->setText( tr( "Close" ) );
        QPixmap pic;
        pic.convertFromImage( Resource::loadImage( "close" ).smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() ) );
        m_btnStart->setIconSet( pic );
        return;
    }
    else if ( btnText == tr( "Close" ) )
    {
        // TODO - force reload of package data
        emit closeInstallDlg();
        return;
    }

    // Start was clicked, start executing
    QString dest;
    if ( m_installFound )
    {
        dest = m_destination->currentText();
        m_destination->setEnabled( false );
    }

    m_btnOptions->setEnabled( false );
    if ( m_numCommands > 1 )
    {
        m_btnStart->setText( tr( "Abort" ) );
        QPixmap pic;
        pic.convertFromImage( Resource::loadImage( "reset" ).smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() ) );
        m_btnStart->setIconSet( pic );
    }
    else
    {
        m_btnStart->setEnabled( false );
    }

    for ( m_currCommand = 0; m_currCommand < m_numCommands; m_currCommand++ )
    {
        // Execute next command
        m_packman->executeCommand( m_command[ m_currCommand ], m_packages[ m_currCommand ], dest,
                                   this, SLOT(slotOutput(const QString &)), true );
    }

    // All commands executed, allow user to close dialog
    m_btnStart->setEnabled( true );
    m_btnStart->setText( tr( "Close" ) );
    QPixmap pic;
    pic.convertFromImage( Resource::loadImage( "close" ).smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() ) );
    m_btnStart->setIconSet( pic );

    m_btnOptions->setEnabled( true );
    m_btnOptions->setText( tr( "Save output" ) );
    pic.convertFromImage( Resource::loadImage( "save" ).smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() ) );
    m_btnOptions->setIconSet( pic );
}

void InstallDlg::slotBtnOptions()
{
    QString btnText = m_btnOptions->text();
    if ( btnText == tr( "Options" ) )
    {
        // Display configuration dialog (only options tab is enabled)
        m_packman->configureDlg( true );
        return;
    }

    // Save output was clicked
    QMap<QString, QStringList> map;
    map.insert( tr( "All" ), QStringList() );
    QStringList text;
    text << "text/*";
    map.insert(tr( "Text" ), text );
    text << "*";
    map.insert( tr( "All" ), text );

    QString filename = Opie::Ui::OFileDialog::getSaveFileName( 2, "/", "ipkg-output", map );
    if( !filename.isEmpty() )
    {
        QString currentFileName = QFileInfo( filename ).fileName();
        DocLnk doc;
        doc.setType( "text/plain" );
        doc.setFile( filename );
        doc.setName( currentFileName );
        FileManager fm;
        fm.saveFile( doc, m_output->text() );
    }
}

void InstallDlg::slotOutput( const QString &msg )
{
    // Allow processing of other events
    qApp->processEvents();

    QString lineStr = msg;
    if ( lineStr[lineStr.length()-1] == '\n' )
        lineStr.truncate( lineStr.length() - 1 );
    m_output->append( lineStr );
    m_output->setCursorPosition( m_output->numLines(), 0 );

    // Update available space
    slotDisplayAvailSpace( QString::null );
}
