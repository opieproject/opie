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

#include <opie/ocontactaccess.h>
#include <qstring.h>

#include "contactitem.h"
//#include <qdir.h>

AdressSearch::AdressSearch(QListView* parent, QString name):
	SearchGroup(parent, name)
{
	_contacts = 0;
}


AdressSearch::~AdressSearch()
{
	delete _contacts;
}


void AdressSearch::expand()
{
	SearchGroup::expand();
	if (_search.isEmpty()) return;
 	if (!_contacts) _contacts = new OContactAccess("osearch");
	ORecordList<OContact> results = _contacts->matchRegexp(_search);
	setText(0, text(0) + " (" + QString::number( results.count() ) + ")" );
	for (uint i = 0; i < results.count(); i++) {
		new ContactItem( this, new OContact( results[i] ));
	}
}

