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

#include "olistviewitem.h"

SearchGroup::SearchGroup(QListView* parent, QString name)
: OListViewItem(parent, name)
{
	_name = name;
	loaded = false;
	expanded = false;
}


SearchGroup::~SearchGroup()
{
}


void SearchGroup::expand()
{
	clearList();
	if (_search.isEmpty()) return;
	OListViewItem *dummy = new OListViewItem( this, "searching...");
	setOpen( expanded );
	if (!loaded) load();
	int res_count = search();
	setText(0, _name + " - " + _search.pattern() + " (" + QString::number( res_count ) + ")");
	delete dummy;
}

void SearchGroup::doSearch()
{
	clearList();
	if (_search.isEmpty()) return;
	if (!loaded) load();
	int res_count = search();
	setText(0, _name + " - " + _search.pattern() + " (" + QString::number( res_count ) + ")");
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
	expanded = true;
}

void SearchGroup::setSearch(QRegExp re)
{
	setText(0, _name+" - "+re.pattern() );
	_search = re;
	if (expanded) expand();
	else new OListViewItem( this, "searching...");
}

