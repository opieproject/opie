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
#include <qpe/qcopenvelope_qws.h>

TodoItem::TodoItem(OListViewItem* parent, OTodo *todo)
: ResultItem(parent)
{
	_todo = todo;
	setText( 0, todo->toShortText() );
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

void TodoItem::showItem()
{
//	QCopEnvelope e("QPE/Todolist", "show(int)");
	QCopEnvelope e("QPE/Application/todolist", "show(int)");
	e << _todo->uid();
}

void TodoItem::editItem()
{
//	QCopEnvelope e("QPE/Todolist", "edit(int)");
	QCopEnvelope e("QPE/Application/todolist", "edit(int)");
	e << _todo->uid();
}
