#include "converter.h"

#include <stdlib.h> // For "system()" command

/* OPIE */
#include <opie2/oapplicationfactory.h>
#include <opie2/odebug.h>
#include <opie2/opimglobal.h>
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
}

void Converter::selectedDatabase( int num )
{
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

void Converter::start_conversion(){

    // Creating backends to the requested databases..
    OPimBase* sourceDB;
    OPimBase* destDB;

    odebug << "SourceFormat: " <<  m_selectedSourceFormat << oendl;
    odebug << "DestFormat: " << m_selectedDestFormat << oendl;
    if ( m_selectedSourceFormat == m_selectedDestFormat ){

	    QMessageBox::warning( this, "PimConverter",
				  tr( "It is not a good idea to use\n" )
				  +tr( "the same source and destformat !" ),
				  tr( "Ok" ) );
	    return;
    }

    switch( m_selectedSourceFormat ){
    case XML:
        odebug << "XMLSourceDB = " << m_selectedDatabase << "" << oendl;
        switch( m_selectedDatabase ){
        case ADDRESSBOOK:{
            sourceDB = OPimAccessFactory<OPimContactAccess>::create( OPimGlobal::CONTACTLIST, OPimGlobal::XML, "converter" );
                }
            break;
        case TODOLIST:{
            sourceDB = OPimAccessFactory<OPimTodoAccess>::create( OPimGlobal::TODOLIST, OPimGlobal::XML, "converter" );
            }break;
        case DATEBOOK:{
            sourceDB = OPimAccessFactory<ODateBookAccess>::create( OPimGlobal::DATEBOOK, OPimGlobal::XML, "converter" );
                }
            break;
        default:
            owarn << "Unknown database selected (" << m_selectedDatabase << ")" << oendl;
            return;
        }
        break;
    case SQL:
        odebug << "SQLSourceDB = " << m_selectedDatabase << "" << oendl;
        switch( m_selectedDatabase ){
        case ADDRESSBOOK:{
            sourceDB = OPimAccessFactory<OPimContactAccess>::create( OPimGlobal::CONTACTLIST, OPimGlobal::SQL, "converter" );
                }
            break;
        case TODOLIST:{
            sourceDB = OPimAccessFactory<OPimTodoAccess>::create( OPimGlobal::TODOLIST, OPimGlobal::SQL, "converter" );
            }break;
        case DATEBOOK:{
            sourceDB = OPimAccessFactory<ODateBookAccess>::create( OPimGlobal::DATEBOOK, OPimGlobal::SQL, "converter" );
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

    switch ( m_selectedDestFormat ){
    case XML:
        odebug << "XMLDestDB = " << m_selectedDatabase << "" << oendl;
        switch( m_selectedDatabase ){
        case ADDRESSBOOK:{
            destDB = OPimAccessFactory<OPimContactAccess>::create( OPimGlobal::CONTACTLIST, OPimGlobal::XML, "converter" );
                }
            break;
        case TODOLIST:{
            destDB = OPimAccessFactory<OPimTodoAccess>::create( OPimGlobal::TODOLIST, OPimGlobal::XML, "converter" );
            }break;
        case DATEBOOK:{
            destDB = OPimAccessFactory<ODateBookAccess>::create( OPimGlobal::DATEBOOK, OPimGlobal::XML, "converter" );
                }
            break;
        default:
            owarn << "Unknown database selected (" << m_selectedDatabase << ")" << oendl;
            return;
        }
        break;
    case SQL:
        odebug << "SQLDestDB = " << m_selectedDatabase << "" << oendl;
        switch( m_selectedDatabase ){
        case ADDRESSBOOK:{
            destDB = OPimAccessFactory<OPimContactAccess>::create( OPimGlobal::CONTACTLIST, OPimGlobal::SQL, "converter" );
                }
            break;
        case TODOLIST:{
            destDB = OPimAccessFactory<OPimTodoAccess>::create( OPimGlobal::TODOLIST, OPimGlobal::SQL, "converter" );
            }break;
        case DATEBOOK:{
            destDB = OPimAccessFactory<ODateBookAccess>::create( OPimGlobal::DATEBOOK, OPimGlobal::SQL, "converter" );
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
    if ( m_eraseDB -> isChecked() ){
        odebug << "Clearing destination database!" << oendl;
        destDB -> clear();
    }

    // Now transmit every pim-item from the source database to the destination -database
    QArray<int> uidList = sourceDB->records();
    odebug << "Try to move data for addressbook.. (" << uidList.count() << " items) " << oendl;
    m_progressBar->setTotalSteps( uidList.count() );
    int count = 0;
    for ( uint i = 0; i < uidList.count(); ++i ){
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
    switch( m_selectedDatabase ){
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
    default:
        owarn << "Unknown database selected (" << m_selectedDatabase << ")" << oendl;
        return;
    }


    owarn << "Conversion is finished and needed " << t.elapsed() << " ms !" << oendl;
}

void Converter::closeEvent( QCloseEvent *e )
{

	/* Due to the fact that we don't have multitasking here, this
	 * critical handling don't make sense, but the future..
	 */
	if ( m_criticalState ){
		e->ignore();
		return;
	}
	e->accept();
}



void Converter::start_upgrade()
{
	odebug << "Start upgrading" << oendl;
	switch( QMessageBox::warning( this, "Pim-Converter",
				      "Are you really sure that you\n"
				      "want to convert your database from\n"
				      "sqlite V2 to sqlite V3?",
				      QMessageBox::Ok | QMessageBox::Default,
				      QMessageBox::Abort | QMessageBox::Escape )) {

	case QMessageBox::Abort: // Abort clicked or Escape pressed
		// abort
		return;
		break;
	}
	odebug << "Checking whether sqlite is installed" << oendl;
	if ( system( "which sqlite" ) != 0 ){
		QMessageBox::critical( this, "Pim-Converter",
				       QString("An internal error occurred:\n") +
				       "sqlite was not accessible!\n"+
				       "Please correct the PATH or install \n" + 
				       "this packages!" );
		return;
	}
	if ( system( "which sqlite3" ) != 0 ){
		QMessageBox::critical( this, "Pim-Converter",
				       QString("An internal error occurred:\n") +
				       "sqlite3 was not accessible!\n"+
				       "Please correct the PATH or install \n" + 
				       "this packages!" );
		return;
	}
	if ( QFile::exists( "~/Applications/addressbook/addressbook.db" ) 
	     && !QFile::exists( "~/Applications/addressbook/addressbook.db_v2" ) ){ 
		odebug << "Executing conversion commands" << oendl;
		QString addr_convert_string = "cd ~/Applications/addressbook/;cp addressbook.db addressbook.db_v2;sqlite addressbook.db_v2 .dump | sqlite3 addressbook.db";
		odebug << "1. Addressbook Command:" << addr_convert_string << oendl;
		if ( system( addr_convert_string ) != 0 ){
			QMessageBox::critical( this, "Pim-Converter",
					       QString("An internal error occurred:\n") +
					       "Converting the addressbook command was impossible!\n"+
					       "Executed the following command:\n" + 
					       addr_convert_string );
			return;
		}
	}
	if ( QFile::exists( "~/Applications/datebook/datebook.db" ) 
	     && !QFile::exists( "~/Applications/datebook/datebook.db_v2" ) ){ 
		QString dateb_convert_string = "cd ~/Applications/datebook/;cp datebook.db datebook.db_v2;sqlite datebook.db_v2 .dump | sqlite3 datebook.db";
		odebug << "2. Datebook Command" << dateb_convert_string << oendl;
		if ( system( dateb_convert_string ) != 0 ){
			QMessageBox::critical( this, "Pim-Converter",
					       QString("An internal error occurred:\n") +
					       "Converting the datebook command was impossible!\n"+
					       "Executed the following command:\n" + 
					       dateb_convert_string );
			return;
		}
	}

	if ( QFile::exists( "~/Applications/todolist/todolist.db" ) 
	     && !QFile::exists( "~/Applications/todolist/todolist.db_v2" ) ){ 
		QString todo_convert_string = "cd ~/Applications/todolist/;cp todolist.db todolist.db_v2;sqlite todolist.db_v2 .dump | sqlite3 todolist.db";
		odebug << "3. Todolist Command:" << todo_convert_string << oendl;
		if ( system( todo_convert_string ) != 0 ){
			QMessageBox::critical( this, "Pim-Converter",
					       QString("An internal error occurred:\n") +
					       "Converting the todolist command was impossible!\n"+
					       "Executed the following command:\n" + 
					       todo_convert_string );
			return;
		}
	}

	QMessageBox::information( this, "Pim-Converter",
				  "Conversion is finished!",
				  "&OK", NULL, NULL,
				  0,      // Enter == button 0
				  0 );
	
	
}
