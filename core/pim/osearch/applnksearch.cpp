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


void AppLnkSearch::load()
{
	_apps = new AppLnkSet(QPEApplication::qpeDir());
}

int AppLnkSearch::search()
{
	int count = 0;
	QList<AppLnk> appList = _apps->children();
	for ( AppLnk *app = appList.first(); app != 0; app = appList.next() ){
		if ( (_search.match( app->name() ) != -1)
		    || (_search.match(app->comment()) != -1)
		    || (_search.match(app->exec()) != -1) ) {
		    	count++;
			insertItem( app );
		}
	}
	return count;
}

void AppLnkSearch::insertItem( void *rec )
{
	new AppLnkItem( this, (AppLnk*)rec );
}

void AppLnkSearch::setSearch(QRegExp re)
{
	expanded = false;
	setOpen( false );
	SearchGroup::setSearch( re );
}

