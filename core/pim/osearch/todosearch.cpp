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
#include "todoitem.h"

#include <qpe/resource.h>
#include <qpe/config.h>

#include <qiconset.h>
#include <qaction.h>
#include <qpopupmenu.h>


TodoSearch::TodoSearch(QListView* parent, QString name)
: SearchGroup(parent, name), _todos(0), _popupMenu(0)
{
//	AppLnkSet als(QPEApplication::qpeDir());
//	setPixmap( 0, als.findExec("todolist")->pixmap() );
	QIconSet is = Resource::loadIconSet( "todo/TodoListSmall" );
	setPixmap( 0, is.pixmap( QIconSet::Large, true ) );
	actionShowCompleted = new QAction( QObject::tr("show completed tasks"),QString::null,  0, 0, 0, true );
	Config cfg( "osearch", Config::User );
   	cfg.setGroup( "todo_settings" );
   	actionShowCompleted->setOn( cfg.readBoolEntry( "show_completed_tasks", false ) );

}


TodoSearch::~TodoSearch()
{
	Config cfg( "osearch", Config::User );
   	cfg.setGroup( "todo_settings" );
   	cfg.writeEntry( "show_completed_tasks", actionShowCompleted->isOn() );
	delete _popupMenu;
	delete actionShowCompleted;
	delete _todos;
}


void TodoSearch::load()
{
	 _todos = new OPimTodoAccess();
	 _todos->load();
}

int TodoSearch::search()
{
	OPimRecordList<OPimTodo> results = _todos->matchRegexp(_search);
	for (uint i = 0; i < results.count(); i++)
		insertItem( new OPimTodo( results[i] ));
	return _resultCount;
}

void TodoSearch::insertItem( void *rec )
{
	OPimTodo *todo = (OPimTodo*)rec;
	if (!actionShowCompleted->isOn() &&
		todo->isCompleted() ) return;
	(void)new TodoItem( this, todo );
	_resultCount++;
}

QPopupMenu* TodoSearch::popupMenu()
{
	if (!_popupMenu){
		_popupMenu = new QPopupMenu( 0 );
 		actionShowCompleted->addTo( _popupMenu );
	}
	return _popupMenu;
}
