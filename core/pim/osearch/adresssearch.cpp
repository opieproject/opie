//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Patrick S. Vogt <tille@handhelds.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "adresssearch.h"
#include "contactitem.h"

#include <qpe/resource.h>

//#include <qwhatsthis.h>

AdressSearch::AdressSearch(QListView* parent, QString name):
	SearchGroup(parent, name)
{
	_contacts = 0;
	QIconSet is = Resource::loadIconSet( "addressbook/AddressBookSmall" );
	setPixmap( 0, is.pixmap( QIconSet::Large, true ) );


//	QWhatsThis::add( this, QObject::tr("Search the addressbook") );
/*	QPixmap pix = Resource::loadPixmap( "addressbook/AddressBook" );
	QImage img = pix.convertToImage();
	img.smoothScale( 14, 14 );
	pix.convertFromImage( img );
	setPixmap( 0, pix );*/
}


AdressSearch::~AdressSearch()
{
	delete _contacts;
}

void AdressSearch::load()
{
	_contacts = new OPimContactAccess("osearch");
}

int AdressSearch::search()
{
	OPimRecordList<OPimContact> results = _contacts->matchRegexp(_search);
	for (uint i = 0; i < results.count(); i++) {
		(void)new ContactItem( this, new OPimContact( results[i] ));
	}
	return results.count();
}

void AdressSearch::insertItem( void* )
{

}
