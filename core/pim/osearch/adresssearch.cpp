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

#include <qstring.h>
#include <qiconset.h>
#include <qwhatsthis.h>
#include <qpe/resource.h>
#include <opie/ocontactaccess.h>

#include "contactitem.h"

AdressSearch::AdressSearch(QListView* parent, QString name):
	SearchGroup(parent, name)
{
	_contacts = 0;
	QIconSet is = Resource::loadIconSet( "addressbook/AddressBook" );
	setPixmap( 0, is.pixmap( QIconSet::Small, true ) );
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
	_contacts = new OContactAccess("osearch");
}

int AdressSearch::search()
{
	ORecordList<OContact> results = _contacts->matchRegexp(_search);
	for (uint i = 0; i < results.count(); i++) {
		new ContactItem( this, new OContact( results[i] ));
	}
	return results.count();
}

void AdressSearch::insertItem( void* )
{

}
