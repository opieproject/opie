#include <qpe/qpeapplication.h>

#include <opie/ocontactaccess.h>
#include <opie/ocontactaccessbackend_xml.h>
#include <opie/ocontactaccessbackend_sql.h>

#include "converter_base.h"

class ConvertXMLToSQL: public converter_base {
public:
	ConvertXMLToSQL()
	{
		convertContact();
	}
private:
	void convertContact();

};


void ConvertXMLToSQL::convertContact(){
	qWarning("Converting Contacts from XML to SQL..");
	
	// Creating backends to the requested databases..
	OContactAccessBackend* xmlBackend = new OContactAccessBackend_XML( "Converter", 
									   QString::null );
	
	OContactAccessBackend* sqlBackend = new OContactAccessBackend_SQL( QString::null,
									   QString::null );
	// Put the created backends into frontends to access them
	OContactAccess* xmlAccess = new OContactAccess ( "addressbook_xml", 
							 QString::null , xmlBackend, true );
	
	OContactAccess* sqlAccess = new OContactAccess ( "addressbook_sql", 
							 QString::null , sqlBackend, true );
	
	// Clean the sql-database..
	sqlAccess->clear();
	
	// Now trasmit every contact from the xml database to the sql-database
	OContactAccess::List contactList = xmlAccess->allRecords();
	if ( sqlAccess && xmlAccess ){
		OContactAccess::List::Iterator it;
		for ( it = contactList.begin(); it != contactList.end(); ++it )
			sqlAccess->add( *it );
	}
	
	// Delete the frontends. Backends will be deleted automatically, too !
	delete sqlAccess;
	delete xmlAccess;
}

int main( int argc, char** argv ) {

	QPEApplication a( argc, argv );

	ConvertXMLToSQL dlg;

	a.showMainWidget( &dlg );
	// dlg. showMaximized ( );

	return a.exec();

}
