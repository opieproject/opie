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
#include "olistviewitem.h"

SearchGroup::SearchGroup(QListView* parent, QString name)
: OListViewItem(parent, name)
{
	_name = name;
	expanded = false;
}


SearchGroup::~SearchGroup()
{
}


void SearchGroup::expand()
{
	QListViewItem *item = firstChild();
	QListViewItem *toDel;

	while ( item != 0 ) {
	 toDel = item;
	 item = item->nextSibling();
	 //takeItem(toDel);
	 delete toDel;
	}
	expanded = true;
}

void SearchGroup::setSearch(QString s)
{
	setSearch( QRegExp( s ) );
	_search.setCaseSensitive(false);
}


void SearchGroup::setSearch(QRegExp re)
{
	setText(0, _name+" - "+re.pattern() );
	_search = re;
	if (expanded) expand();
	else new OListViewItem( this, "searching...");
}

