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
#include "todosearch.h"

#include <opie/otodoaccess.h>
#include <opie/otodo.h>

#include "todoitem.h"

TodoSearch::TodoSearch(QListView* parent, QString name)
: SearchGroup(parent, name)
{
	_todos = 0;
}


TodoSearch::~TodoSearch()
{
	delete _todos;
}


void TodoSearch::load()
{
	 _todos = new OTodoAccess();
	 _todos->load();
}

int TodoSearch::search()
{
	ORecordList<OTodo> results = _todos->matchRegexp(_search);
	for (uint i = 0; i < results.count(); i++)
		new TodoItem( this, new OTodo( results[i] ));
	return results.count();
}

void TodoSearch::insertItem( void* )
{

}
