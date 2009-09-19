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

#include <opie2/oresource.h>

#include <qpe/config.h>

#include <qaction.h>
#include <qpopupmenu.h>

using namespace Opie;
TodoSearch::TodoSearch(QListView* parent, QString name)
: SearchGroup(parent, name), _todos(0), _popupMenu(0)
{
//    AppLnkSet als(QPEApplication::qpeDir());
//    setPixmap( 0, als.findExec("todolist")->pixmap() );
    setPixmap( 0, Opie::Core::OResource::loadPixmap( "todo/TodoList", Opie::Core::OResource::SmallIcon ) );

    actionShowCompleted = new QAction( QObject::tr("Show completed tasks"),QString::null,  0, 0, 0, true );
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


bool TodoSearch::load()
{
    _todos = new OPimTodoAccess();
    return _todos->load();
}

int TodoSearch::search()
{
    OPimRecordList<OPimTodo> results = _todos->matchRegexp(m_search);
    for (uint i = 0; i < results.count(); i++)
        insertItem( new OPimTodo( results[i] ));
    return m_resultCount;
}

void TodoSearch::insertItem( void *rec )
{
    OPimTodo *todo = (OPimTodo*)rec;
    if (!actionShowCompleted->isOn() &&
        todo->isCompleted() ) return;
    (void)new TodoItem( this, todo );
    m_resultCount++;
}

QPopupMenu* TodoSearch::popupMenu()
{
    if (!_popupMenu){
        _popupMenu = new QPopupMenu( 0 );
        actionShowCompleted->addTo( _popupMenu );
    }
    return _popupMenu;
}
