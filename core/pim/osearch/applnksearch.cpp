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
#include <qiconset.h>
#include <qpe/resource.h>

#include "applnkitem.h"

AppLnkSearch::AppLnkSearch(QListView* parent, QString name): SearchGroup(parent, name)
{
	_apps = 0;
	QIconSet is = Resource::loadIconSet( "osearch/applications" );
	//QIconSet is = Resource::loadIconSet( "AppsIcon" );
	setPixmap( 0, is.pixmap( QIconSet::Small, true ) );
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
	QList<AppLnk> appList = _apps->children();
	for ( AppLnk *app = appList.first(); app != 0; app = appList.next() ){
		if ( (_search.match( app->name() ) != -1)
		    || (_search.match(app->comment()) != -1)
		    || (_search.match(app->exec()) != -1) ) {
			insertItem( app );
		}
		qApp->processEvents( 100 );
	}
	return _resultCount;
}

void AppLnkSearch::insertItem( void *rec )
{
	new AppLnkItem( this, (AppLnk*)rec );
	_resultCount++;
}

void AppLnkSearch::setSearch(QRegExp re)
{
	setOpen( false );
	SearchGroup::setSearch( re );
}

