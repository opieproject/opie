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
#include <qdir.h>

#include "todoitem.h"

TodoSearch::TodoSearch(QListView* parent, QString name)
: SearchGroup(parent, name)
{
	//_fileName = QDir::homeDirPath()+"/Applications/todolist/todolist.xml";
	_todos = 0;
}


TodoSearch::~TodoSearch()
{
	delete _todos;
}


void TodoSearch::expand()
{
	SearchGroup::expand();
	if (_search.isEmpty()) return;

	if (!_todos){
	 _todos = new OTodoAccess();
	 _todos->load();
	 }

	ORecordList<OTodo> results = _todos->matchRegexp(_search);
	for (uint i = 0; i < results.count(); i++) {
		new TodoItem( this, new OTodo( results[i] ));
	}
/*
	ORecordList<OTodo> list = _todos->allRecords();
       QArray<int> m_currentQuery( list.count() );
       for( uint i=0; i<list.count(); i++ ){
     //  	qDebug("todo: %s",list[i].summary().latin1() );
                 if ( list[i].match( _search ) ){
	//	 	qDebug("FOUND");
			new TodoItem( this, new OTodo( list[i] ) );
                 }

        }*/

}


