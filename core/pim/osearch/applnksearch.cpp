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
#include "applnksearch.h"

#include <qpe/applnk.h>
#include <qpe/qpeapplication.h>

#include "applnkitem.h"

AppLnkSearch::AppLnkSearch(QListView* parent, QString name): SearchGroup(parent, name)
{
	_apps = 0;
}


AppLnkSearch::~AppLnkSearch()
{
	delete _apps;
}


void AppLnkSearch::expand()
{
	SearchGroup::expand();
	if (_search.isEmpty()) return;
	if (!_apps) _apps = new AppLnkSet(QPEApplication::qpeDir());
	QList<AppLnk> appList = _apps->children();
	for ( AppLnk *app = appList.first(); app != 0; app = appList.next() ){
//		if (app->name().contains(_search) || app->comment().contains(_search))
		if ( (_search.match( app->name() ) != -1)
		    || (_search.match(app->comment()) != -1)
		    || (_search.match(app->exec()) != -1) )
			new AppLnkItem( this, app );
	}
}

