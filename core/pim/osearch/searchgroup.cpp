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
#include "searchgroup.h"

#include <qregexp.h>
// #include <qapplication.h>
// #include <opie/owait.h>

#include "olistviewitem.h"

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
	clearList();
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
	int res_count = realSearch();
	setText(0, _name + " - " + _search.pattern() + " (" + QString::number( res_count ) + ")");
//	repaint();
}

void SearchGroup::clearList()
{
	QListViewItem *item = firstChild();
	QListViewItem *toDel;
	while ( item != 0 ) {
		toDel = item;
		item = item->nextSibling();
	 	delete toDel;
	}
}

void SearchGroup::setSearch(QRegExp re)
{
	setText(0, _name+" - "+re.pattern() );
	_search = re;
	if (isOpen()) expand();
	else new OListViewItem( this, "searching...");
}

int SearchGroup::realSearch()
{
	//emit isSearching( tr(" Searching for %s in %s" ).arg( _search.pattern().latin1()).arg( _name ) );
/*	OWait *wait = new OWait( qApp->mainWidget(), "test" );
	wait->show();*/
	if (!loaded) load();
	int count = search();
/*	wait->hide();
	delete wait;*/
	return count;
}

