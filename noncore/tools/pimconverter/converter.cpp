#include "converter.h"

#include <qdatetime.h>
#include <qprogressbar.h>
#include <qcombobox.h>
#include <qcheckbox.h>

#include <qpe/qpeapplication.h>

#include <opie2/ocontactaccess.h>
#include <opie2/ocontactaccessbackend_xml.h>
#include <opie2/ocontactaccessbackend_sql.h>

#include <opie2/otodoaccess.h>
#include <opie2/otodoaccessxml.h>
#include <opie2/otodoaccesssql.h>

#include <opie2/odatebookaccess.h>
#include <opie2/odatebookaccessbackend_xml.h>
#include <opie2/odatebookaccessbackend_sql.h>

// #define _ADDRESSBOOK_ACCESS

using namespace Opie;

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
			OPimContactAccessBackend* sourceBackend = new OPimContactAccessBackend_XML( "Converter", QString::null );
			sourceDB = new OPimContactAccess ( "addressbook_xml", QString::null , sourceBackend, true );
		        } 
			break;
		case TODOLIST:{
			OPimTodoAccessBackend* sourceBackend = new OPimTodoAccessXML( "Converter" );
			sourceDB = new OPimTodoAccess( sourceBackend );
			}break;
		case DATEBOOK:{
			ODateBookAccessBackend_XML* sourceBackend = new ODateBookAccessBackend_XML( "Converter", QString::null );
			sourceDB = new ODateBookAccess ( sourceBackend );
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
			qDebug("SQLSourceDB = %d", m_selectedDatabase);
			OPimContactAccessBackend* sourceBackend = new OPimContactAccessBackend_SQL( QString::null, QString::null );
			sourceDB =  new OPimContactAccess ( "Converter", QString::null, sourceBackend, true );
			}
			break;
		case TODOLIST:{
			OPimTodoAccessBackend* sourceBackend = new OPimTodoAccessBackendSQL( QString::null );
			sourceDB = new OPimTodoAccess( sourceBackend );
			}break;
		case DATEBOOK: {
			ODateBookAccessBackend_SQL* sourceBackend = new ODateBookAccessBackend_SQL( "Converter", QString::null );
			sourceDB = new ODateBookAccess ( sourceBackend );
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
			OPimContactAccessBackend* destBackend = new OPimContactAccessBackend_XML( "Converter", QString::null );
			destDB = new OPimContactAccess ( "Converter", QString::null , destBackend, true );
			}
			break;
		case TODOLIST:{
			OPimTodoAccessBackend* destBackend = new OPimTodoAccessXML( "Converter" );
			destDB = new OPimTodoAccess( destBackend );
			}break;
		case DATEBOOK:{
			ODateBookAccessBackend_XML* destBackend = new ODateBookAccessBackend_XML( "Converter", QString::null );
			destDB = new ODateBookAccess ( destBackend );
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
			OPimContactAccessBackend* destBackend = new OPimContactAccessBackend_SQL( QString::null, QString::null );
			destDB = new OPimContactAccess ( "addressbook_xml", QString::null , destBackend, true );
			}
			break;
		case TODOLIST:{
			OPimTodoAccessBackend* destBackend = new OPimTodoAccessBackendSQL( QString::null );
			destDB = new OPimTodoAccess( destBackend );
			}break;
		case DATEBOOK:{
			ODateBookAccessBackend_SQL* destBackend = new ODateBookAccessBackend_SQL( "Converter", QString::null );
			destDB = new ODateBookAccess ( destBackend );
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
