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
#include "datebooksearch.h"

#include "eventitem.h"

#include <opie/oevent.h>
#include <opie/odatebookaccess.h>
#include <qiconset.h>
#include <qpe/resource.h>

DatebookSearch::DatebookSearch(QListView* parent, QString name)
: SearchGroup(parent, name)
{
	_dates = 0;
	QIconSet is = Resource::loadIconSet( "datebook/DateBook" );
	setPixmap( 0, is.pixmap( QIconSet::Small, true ) );

}


DatebookSearch::~DatebookSearch()
{
	delete _dates;
}


void DatebookSearch::load()
{
	 _dates = new ODateBookAccess();
	 _dates->load();
}

int DatebookSearch::search()
{
	ORecordList<OEvent> results = _dates->matchRegexp(_search);
	for (uint i = 0; i < results.count(); i++)
		new EventItem( this, new OEvent( results[i] ));
	return results.count();
}

void DatebookSearch::insertItem( void* )
{

}
