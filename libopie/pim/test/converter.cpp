#include "converter.h"

#include <qdatetime.h>
#include <qprogressbar.h>

#include <qpe/qpeapplication.h>

#include <opie/ocontactaccess.h>
#include <opie/ocontactaccessbackend_xml.h>
#include <opie/ocontactaccessbackend_sql.h>

#include <opie/odatebookaccess.h>
#include <opie/odatebookaccessbackend_xml.h>
#include <opie/odatebookaccessbackend_sql.h>

// #define _ADDRESSBOOK_ACCESS

Converter::Converter(){
}

void Converter::start_conversion(){
	qWarning("Converting Contacts from XML to SQL..");
	
	// Creating backends to the requested databases..

#ifdef _ADDRESSBOOK_ACCESS
	OContactAccessBackend* xmlBackend = new OContactAccessBackend_XML( "Converter", 
									   QString::null );
	
	OContactAccessBackend* sqlBackend = new OContactAccessBackend_SQL( QString::null,
									   QString::null );
	// Put the created backends into frontends to access them
	OContactAccess* xmlAccess = new OContactAccess ( "addressbook_xml", 
							 QString::null , xmlBackend, true );
	
	OContactAccess* sqlAccess = new OContactAccess ( "addressbook_sql", 
							 QString::null );
	
#else
	ODateBookAccessBackend* xmlBackend = new ODateBookAccessBackend_XML( "Converter", 
									     QString::null );
	
	ODateBookAccessBackend* sqlBackend = new ODateBookAccessBackend_SQL( QString::null,
									     QString::null );
	// Put the created backends into frontends to access them
	ODateBookAccess* xmlAccess = new ODateBookAccess ( xmlBackend );
	
	ODateBookAccess* sqlAccess = new ODateBookAccess ( sqlBackend );
	
	xmlAccess->load();

#endif

	QTime t;
	t.start();

// Clean the sql-database..
	sqlAccess->clear();
	
#ifdef _ADDRESSBOOK_ACCESS
	// Now trasmit every contact from the xml database to the sql-database
	OContactAccess::List contactList = xmlAccess->allRecords();
	m_progressBar->setTotalSteps( contactList.count() );
	int count = 0;
	if ( sqlAccess && xmlAccess ){
		OContactAccess::List::Iterator it;
		for ( it = contactList.begin(); it != contactList.end(); ++it ){
			sqlAccess->add( *it );
			m_progressBar->setProgress( ++count );
		}
	}
#else
	// Now transmit every contact from the xml database to the sql-database
	ODateBookAccess::List dateList = xmlAccess->allRecords();
	m_progressBar->setTotalSteps( dateList.count() );
	qWarning( "Number of elements to copy: %d", dateList.count() );

	int count = 0;
	if ( sqlAccess && xmlAccess ){
		ODateBookAccess::List::Iterator it;
		for ( it = dateList.begin(); it != dateList.end(); ++it ){
			sqlAccess->add( *it );
			m_progressBar->setProgress( ++count );
		}
	}

#endif	
	// Delete the frontends. Backends will be deleted automatically, too !
	delete sqlAccess;

	qWarning("Conversion is finished and needed %d ms !", t.elapsed());

	delete xmlAccess;
}

int main( int argc, char** argv ) {

	QPEApplication a( argc, argv );

	Converter dlg;

	a.showMainWidget( &dlg );
	// dlg. showMaximized ( );

	return a.exec();

}
