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
#include "todoitem.h"

#include <opie/otodo.h>

TodoItem::TodoItem(OListViewItem* parent, OTodo *todo)
: ResultItem(parent)
{
	_todo = todo;
	setText( 0, todo->summary() );
}


TodoItem::~TodoItem()
{
	delete _todo;
}


void TodoItem::expand()
{
    ResultItem::expand();
}

QString TodoItem::toRichText()
{
	return _todo->toRichText();
}

