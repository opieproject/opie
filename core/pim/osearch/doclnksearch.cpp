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
#include "doclnksearch.h"

#include <qpe/applnk.h>
#include <qpe/qpeapplication.h>

#include "doclnkitem.h"

DocLnkSearch::DocLnkSearch(QListView* parent, QString name): SearchGroup(parent, name)
{
	_docs = 0;
}


DocLnkSearch::~DocLnkSearch()
{
	delete _docs;
}


void DocLnkSearch::expand()
{
	SearchGroup::expand();
	if (_search.isEmpty()) return;
	if (!_docs) _docs = new DocLnkSet(QPEApplication::documentDir());
	QList<DocLnk> appList = _docs->children();
	for ( DocLnk *app = appList.first(); app != 0; app = appList.next() ){
//		if (app->name().contains(_search) || app->comment().contains(_search))
		if ( (_search.match( app->name() ) != -1)
		    || (_search.match(app->comment()) != -1)
		    || (_search.match(app->exec()) != -1) )
			new DocLnkItem( this, app );
	}
}

