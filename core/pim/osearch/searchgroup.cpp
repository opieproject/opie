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
#include "olistviewitem.h"
#include "searchgroup.h"

#include <opie2/owait.h>

#include <qobject.h>
#include <qregexp.h>
#include <qapplication.h>

#ifndef NEW_OWAIT
static OWait *wait = 0;
#endif

SearchGroup::SearchGroup(QListView* parent, QString name)
: OListViewItem(parent, name)
{
	_name = name;
	loaded = false;
}


SearchGroup::~SearchGroup()
{
}


void SearchGroup::expand()
{
	//expanded = true;
	if (_lastSearch != _search) clearList();
	if (_search.isEmpty()) return;
	OListViewItem *dummy = new OListViewItem( this, "searching...");
	setOpen( true );
	repaint();
	int res_count = realSearch();
	setText(0, _name + " - " + _search.pattern() + " (" + QString::number( res_count ) + ")");
	delete dummy;
	repaint();
}

void SearchGroup::doSearch()
{
	clearList();
	if (_search.isEmpty()) return;
	_resultCount = realSearch();
//	repaint();
}

void SearchGroup::setSearch(QRegExp re)
{
	if (re == _search) return;
	setText(0, _name+" - "+re.pattern() );
	_search = re;
	if (isOpen()) expand();
	else new OListViewItem( this, "searching...");
}

int SearchGroup::realSearch()
{
	if (_lastSearch == _search) return _resultCount;
#ifndef NEW_OWAIT
	if (!wait) wait = new OWait( qApp->mainWidget(), "osearch" );
	wait->show();
	qApp->processEvents();
#else
	qDebug("********** NEW_OWAIT *************");
	OWait( "searching" );
#endif
	if (!loaded) load();
	_resultCount = 0;
	_resultCount = search();
	_lastSearch = _search;
	setText(0, _name + " - " + _search.pattern() + " (" + QString::number( _resultCount ) + ")");

#ifndef NEW_OWAIT
	wait->hide();
#endif
	return _resultCount;
}

