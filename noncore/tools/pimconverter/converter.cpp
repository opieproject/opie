#include "converter.h"

#include <qdatetime.h>
#include <qprogressbar.h>
#include <qcombobox.h>
#include <qcheckbox.h>

#include <qpe/qpeapplication.h>

#include <opie2/opimglobal.h>
// Include SQL related header files
#define __USE_SQL
#include <opie2/opimaccessfactory.h>

using namespace Opie;
using namespace Pim;

Converter::Converter():
	m_selectedDatabase( ADDRESSBOOK ), 
	m_selectedSourceFormat( XML ),
	m_selectedDestFormat( SQL )
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

	switch( m_selectedSourceFormat ){
	case XML:
		qDebug("XMLSourceDB = %d", m_selectedDatabase);
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
			qWarning( "Unknown database selected (%d)", m_selectedDatabase );
			return;
		}
		break;
	case SQL:
		qDebug("SQLSourceDB = %d", m_selectedDatabase);
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
			qWarning( "Unknown database selected (%d)", m_selectedDatabase );
			return;
		}
		break;
	default:
		qWarning( "Unknown source format selected (%d) !!", m_selectedSourceFormat );
		return;
	}

	switch ( m_selectedDestFormat ){
	case XML:
		qDebug("XMLDestDB = %d", m_selectedDatabase);
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
			qWarning( "Unknown database selected (%d)", m_selectedDatabase );
			return;
		}
		break;
	case SQL:
		qDebug("SQLDestDB = %d", m_selectedDatabase);
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
			qWarning( "Unknown database selected (%d)", m_selectedDatabase );
			return;
		}
		break;
	default:
		qWarning( "Unknown destination format selected (%d)!!", m_selectedDestFormat );
		return;
	}

	if ( !sourceDB || !destDB )
		return;

	sourceDB -> load();
	destDB -> load();

	QTime t;
	t.start();

	// Clean the dest-database if requested (isChecked) 
	if ( m_eraseDB -> isChecked() ){
		qDebug( "Clearing destination database!" );
		destDB -> clear();
	}

	// Now transmit every pim-item from the source database to the destination -database
	QArray<int> uidList = sourceDB->records();
	qDebug( "Try to move data for addressbook.. (%d items) ", uidList.count() );
	m_progressBar->setTotalSteps( uidList.count() );
	int count = 0;
	for ( uint i = 0; i < uidList.count(); ++i ){
		qDebug( "Adding uid: %d", uidList[i] );
		OPimRecord* rec = sourceDB -> record( uidList[i] );
		destDB -> add( rec );
		m_progressBar->setProgress( ++count );
	}

	// Now commit data..
	destDB -> save();

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
		qWarning( "Unknown database selected (%d)", m_selectedDatabase );
		return;
	}
	

	qWarning("Conversion is finished and needed %d ms !", t.elapsed());
}

int main( int argc, char** argv ) {

	QPEApplication a( argc, argv );

	Converter dlg;

	a.showMainWidget( &dlg );
	// dlg. showMaximized ( );

	return a.exec();

}
