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
#include <qiconset.h>
#include <qpe/resource.h>

#include "doclnkitem.h"

DocLnkSearch::DocLnkSearch(QListView* parent, QString name)
: AppLnkSearch(parent, name)
{
	QIconSet is = Resource::loadIconSet( "osearch/documents" );
	setPixmap( 0, is.pixmap( QIconSet::Small, true ) );
}


DocLnkSearch::~DocLnkSearch()
{
}

void DocLnkSearch::load()
{
	_apps = new DocLnkSet(QPEApplication::documentDir());
}

void DocLnkSearch::insertItem( void *rec )
{
	new DocLnkItem( this, (DocLnk*)rec );
	_resultCount++;
}
