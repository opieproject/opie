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

DatebookSearch::DatebookSearch(QListView* parent, QString name)
: SearchGroup(parent, name)
{
	_dates = 0;
}


DatebookSearch::~DatebookSearch()
{
	delete _dates;
}


void DatebookSearch::expand()
{
	SearchGroup::expand();
 	if (!_dates){
	 _dates = new ODateBookAccess();
	 _dates->load();
	 }
#ifdef LIPBOPIE_SEARCH
	ORecordList<OEvent> results = _dates->matchRegexp(_search);
	setText(0, text(0) + " (" + QString::number( results.count() ) + ")" );
	for (uint i = 0; i < results.count(); i++) {
		new EventItem( this, new OEvent( results[i] ));
	}
#else
	ORecordList<OEvent> list = _dates->allRecords();
       QArray<int> m_currentQuery( list.count() );
       for( uint i=0; i<list.count(); i++ ){
	//qDebug("datebook: %s",list[i].description().latin1() );
                 if (
		 	( list[i].description().contains( _search )  ) ||
		 	( list[i].note().contains( _search )  ) ||
		 	( list[i].location().contains( _search )  )
		 ){
		 	//qDebug("FOUND");
			new EventItem( this, new OEvent( list[i] ) );
                 }

        }
#endif
}

