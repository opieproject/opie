/*
                             This file is part of the Opie Project
                             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "converter.h"

#include <stdlib.h> // For "system()" command

/* OPIE */
#include <opie2/oapplicationfactory.h>
#include <opie2/odebug.h>
#include <opie2/opimglobal.h>
#include <qpe/global.h>
#include <qpe/config.h>
// Include SQL related header files
#define __USE_SQL
#include <opie2/opimaccessfactory.h>

/* QT */
#include <qdatetime.h>
#include <qprogressbar.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qmessagebox.h>


OPIE_EXPORT_APP( Opie::Core::OApplicationFactory<Converter> )

using namespace Opie;
using namespace Pim;

Converter::Converter(QWidget *p, const char* n,  WFlags fl):
    converter_base( p, n, fl ),
    m_selectedDatabase( ADDRESSBOOK ),
    m_selectedSourceFormat( XML ),
    m_selectedDestFormat( SQL ),
    m_criticalState( false )
{
    m_dataBaseSelector -> setCurrentItem( m_selectedDatabase );
    m_sourceFormatSelector -> setCurrentItem( m_selectedSourceFormat );
    m_destFormatSelector -> setCurrentItem( m_selectedDestFormat );
    m_eraseDB -> setChecked( true );  // Default erase on copy

    loadPimAccess();
}

void Converter::loadPimAccess()
{
    QString dbtype;

    Config config( "pimaccess" );

    config.setGroup( "datebook" );
    dbtype = config.readEntry( "usebackend", "sql" );
    if( dbtype == "sql" )
        m_datebookFormatSelector->setCurrentItem( 1 );
    else
        m_datebookFormatSelector->setCurrentItem( 0 );

    config.setGroup( "contact" );
    dbtype = config.readEntry( "usebackend", "sql" );
    if( dbtype == "sql" )
        m_contactsFormatSelector->setCurrentItem( 1 );
    else
        m_contactsFormatSelector->setCurrentItem( 0 );

    config.setGroup( "todo" );
    dbtype = config.readEntry( "usebackend", "sql" );
    if( dbtype == "sql" )
        m_todoFormatSelector->setCurrentItem( 1 );
    else
        m_todoFormatSelector->setCurrentItem( 0 );

    config.setGroup( "notes" );
    dbtype = config.readEntry( "usebackend", "sql" );
    if( dbtype == "sql" )
        m_notesFormatSelector->setCurrentItem( 1 );
    else
        m_notesFormatSelector->setCurrentItem( 0 );
}

void Converter::savePimAccess()
{
    Config config( "pimaccess" );

    config.setGroup( "datebook" );
    if( m_datebookFormatSelector->currentItem() == 1 )
        config.writeEntry( "usebackend", "sql" );
    else
        config.writeEntry( "usebackend", "xml" );

    config.setGroup( "contact" );
    if( m_contactsFormatSelector->currentItem() == 1 )
        config.writeEntry( "usebackend", "sql" );
    else
        config.writeEntry( "usebackend", "xml" );

    config.setGroup( "todo" );
    if( m_todoFormatSelector->currentItem() == 1 )
        config.writeEntry( "usebackend", "sql" );
    else
        config.writeEntry( "usebackend", "xml" );

    config.setGroup( "notes" );
    if( m_notesFormatSelector->currentItem() == 1 )
        config.writeEntry( "usebackend", "sql" );
    else
        config.writeEntry( "usebackend", "text" );
}

void Converter::selectedDatabase( int num )
{
    if( num == 3 ) {
        // Memos
        m_sourceFormatSelector->changeItem( tr("Plain text"), 0 );
        m_destFormatSelector->changeItem( tr("Plain text"), 0 );
    }
    else {
        // Everything else
        m_sourceFormatSelector->changeItem( tr("XML"), 0 );
        m_destFormatSelector->changeItem( tr("XML"), 0 );
    }
    m_selectedDatabase = num;
}

void Converter::selectedDestFormat( int num )
{
    m_selectedDestFormat = num;
}

void Converter::selectedSourceFormat( int num )
{
    m_selectedSourceFormat = num;
}

void Converter::start_conversion()
{
    // Creating backends to the requested databases..
    OPimBase* sourceDB;
    OPimBase* destDB;

    odebug << "SourceFormat: " <<  m_selectedSourceFormat << oendl;
    odebug << "DestFormat: " << m_selectedDestFormat << oendl;
    if ( m_selectedSourceFormat == m_selectedDestFormat ) {

        QMessageBox::warning( this, tr("PimConverter"),
                  tr( "<qt>It is not a good idea to use"
                      "the same source and destformat !</qt>"));
        return;
    }

    switch( m_selectedSourceFormat ) {
        case XML:
            odebug << "XMLSourceDB = " << m_selectedDatabase << "" << oendl;
            switch( m_selectedDatabase ) {
                case ADDRESSBOOK:
                    {
                        sourceDB = OPimAccessFactory<OPimContactAccess>::create( OPimGlobal::CONTACTLIST, OPimGlobal::XML, "converter" );
                    }
                    break;
                case TODOLIST:
                    {
                        sourceDB = OPimAccessFactory<OPimTodoAccess>::create( OPimGlobal::TODOLIST, OPimGlobal::XML, "converter" );
                    }
                    break;
                case DATEBOOK:
                    {
                        sourceDB = OPimAccessFactory<ODateBookAccess>::create( OPimGlobal::DATEBOOK, OPimGlobal::XML, "converter" );
                    }
                    break;
                case NOTES:
                    {
                        sourceDB = OPimAccessFactory<OPimMemoAccess>::create( OPimGlobal::NOTES, OPimGlobal::TEXT, "converter" );
                    }
                    break;
                default:
                    owarn << "Unknown database selected (" << m_selectedDatabase << ")" << oendl;
                    return;
            }
            break;
        case SQL:
            odebug << "SQLSourceDB = " << m_selectedDatabase << "" << oendl;
            switch( m_selectedDatabase ) {
                case ADDRESSBOOK:
                    {
                        sourceDB = OPimAccessFactory<OPimContactAccess>::create( OPimGlobal::CONTACTLIST, OPimGlobal::SQL, "converter" );
                    }
                    break;
                case TODOLIST:
                    {
                        sourceDB = OPimAccessFactory<OPimTodoAccess>::create( OPimGlobal::TODOLIST, OPimGlobal::SQL, "converter" );
                    }
                    break;
                case DATEBOOK:
                    {
                        sourceDB = OPimAccessFactory<ODateBookAccess>::create( OPimGlobal::DATEBOOK, OPimGlobal::SQL, "converter" );
                    }
                    break;
                case NOTES:
                    {
                        sourceDB = OPimAccessFactory<OPimMemoAccess>::create( OPimGlobal::NOTES, OPimGlobal::SQL, "converter" );
                    }
                    break;
                default:
                    owarn << "Unknown database selected (" << m_selectedDatabase << ")" << oendl;
                    return;
            }
            break;
        default:
            owarn << "Unknown source format selected (" << m_selectedSourceFormat << ") !!" << oendl;
            return;
    }

    switch ( m_selectedDestFormat ) {
        case XML:
            odebug << "XMLDestDB = " << m_selectedDatabase << "" << oendl;
            switch( m_selectedDatabase ) {
                case ADDRESSBOOK:
                    {
                        destDB = OPimAccessFactory<OPimContactAccess>::create( OPimGlobal::CONTACTLIST, OPimGlobal::XML, "converter" );
                    }
                    break;
                case TODOLIST:
                    {
                        destDB = OPimAccessFactory<OPimTodoAccess>::create( OPimGlobal::TODOLIST, OPimGlobal::XML, "converter" );
                    }
                    break;
                case DATEBOOK:
                    {
                        destDB = OPimAccessFactory<ODateBookAccess>::create( OPimGlobal::DATEBOOK, OPimGlobal::XML, "converter" );
                    }
                    break;
                case NOTES:
                    {
                        destDB = OPimAccessFactory<OPimMemoAccess>::create( OPimGlobal::NOTES, OPimGlobal::TEXT, "converter" );
                    }
                    break;
                default:
                    owarn << "Unknown database selected (" << m_selectedDatabase << ")" << oendl;
                    return;
            }
            break;
        case SQL:
            odebug << "SQLDestDB = " << m_selectedDatabase << "" << oendl;
            switch( m_selectedDatabase ) {
                case ADDRESSBOOK:
                    {
                        destDB = OPimAccessFactory<OPimContactAccess>::create( OPimGlobal::CONTACTLIST, OPimGlobal::SQL, "converter" );
                    }
                    break;
                case TODOLIST:
                    {
                        destDB = OPimAccessFactory<OPimTodoAccess>::create( OPimGlobal::TODOLIST, OPimGlobal::SQL, "converter" );
                    }
                    break;
                case DATEBOOK:
                    {
                        destDB = OPimAccessFactory<ODateBookAccess>::create( OPimGlobal::DATEBOOK, OPimGlobal::SQL, "converter" );
                    }
                    break;
                case NOTES:
                    {
                        destDB = OPimAccessFactory<OPimMemoAccess>::create( OPimGlobal::NOTES, OPimGlobal::SQL, "converter" );
                    }
                    break;
                default:
                    owarn << "Unknown database selected (" << m_selectedDatabase << ")" << oendl;
                    return;
            }
            break;
        default:
            owarn << "Unknown destination format selected (" << m_selectedDestFormat << ")!!" << oendl;
            return;
    }

    if ( !sourceDB || !destDB )
        return;

    m_criticalState = true;

    sourceDB -> load();
    destDB -> load();

    QTime t;
    t.start();

    // Clean the dest-database if requested (isChecked)
    if ( m_eraseDB -> isChecked() ) {
        odebug << "Clearing destination database!" << oendl;
        destDB -> clear();
    }

    // Now transmit every pim-item from the source database to the destination -database
    QArray<int> uidList = sourceDB->records();
    odebug << "Try to move data for addressbook.. (" << uidList.count() << " items) " << oendl;
    m_progressBar->setTotalSteps( uidList.count() );
    int count = 0;
    for ( uint i = 0; i < uidList.count(); ++i ) {
        odebug << "Adding uid: " << uidList[i] << "" << oendl;
        OPimRecord* rec = sourceDB -> record( uidList[i] );
        destDB -> add( rec );
        m_progressBar->setProgress( ++count );
    }

    // Now commit data..
    destDB -> save();

    m_criticalState = false;

    // Delete the frontends. Backends will be deleted automatically, too !
    // We have to cast them back to delete them properly !
    switch( m_selectedDatabase ) {
        case ADDRESSBOOK:
            delete static_cast<OPimContactAccess*> (sourceDB);
            delete static_cast<OPimContactAccess*> (destDB);
            break;
        case TODOLIST:
            delete static_cast<OPimTodoAccess*> (sourceDB);
            delete static_cast<OPimTodoAccess*> (destDB);
            break;
        case DATEBOOK:
            delete static_cast<ODateBookAccess*> (sourceDB);
            delete static_cast<ODateBookAccess*> (destDB);
            break;
        case NOTES:
            delete static_cast<OPimMemoAccess*> (sourceDB);
            delete static_cast<OPimMemoAccess*> (destDB);
            break;
        default:
            owarn << "Unknown database selected (" << m_selectedDatabase << ")" << oendl;
            return;
    }

    owarn << "Conversion is finished and needed " << t.elapsed() << " ms !" << oendl;
}

void Converter::closeEvent( QCloseEvent *e )
{
    savePimAccess();

    /* Due to the fact that we don't have multitasking here, this
     * critical handling don't make sense, but the future..
     */
    if ( m_criticalState ) {
        e->ignore();
        return;
    }
    e->accept();
}

bool Converter::sqliteMoveAndConvert( const QString& name, const QString& src,
                                      const QString &dest )
{
    QMessageBox::information(
                    this, tr( "Pim-Converter" ),
                    tr( "<qt>Starting convert of database %1</qt>" ).arg( name )
                            );

    bool error = false;
    QString cmd;
    if (!QFile::exists( src ) ) {
        cmd = tr( "No SQLite2 database could be found!" );
        error = true;
    }

    if( QFile::exists( dest ) ) {
        cmd = tr( "The database is already converted!" );
        error = true;
    }

    if ( error ) {
        QMessageBox::critical( this, tr("Pim-Converter"),
                               tr("<qt>Conversion not possible: <br>"
                                  "Problem: %1</qt>").arg(cmd) );
        return error;
    }

    /*
     * Move it over
     */
    cmd = "mv " + Global::shellQuote(src) + " " + Global::shellQuote(dest);
    if( ::system( cmd ) != 0 ) {
        error = true;
    }

    /*
     * Convert it
     */
    if ( !error ) {
        cmd = "sqlite " + Global::shellQuote(dest) + " .dump | sqlite3 "
              + Global::shellQuote(src);
        if ( ::system( cmd ) != 0 )
            error = true;
    }

    /*
     * Check whether conversion really worked. If not, move old database back to
     * recover it
     */
    if ( !QFile::exists( src ) ) {
        cmd = "mv " + Global::shellQuote(dest) + " " + Global::shellQuote(src);
        if ( ::system( cmd ) != 0 ) {
            error = true;
            cmd = "Database-Format is not V2!?";
        }
    }

    if ( error ) {
        QMessageBox::critical( this, tr("Pim-Converter"),
                               tr("<qt>An internal error occurred: <br>"
                                  "Converting the database was impossible! <br>"
                                  "Command/Reason: '%1' </qt>").arg(cmd) );
    }
    return error;
}
