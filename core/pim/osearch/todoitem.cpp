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


#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>



using namespace Opie;
TodoItem::TodoItem(OListViewItem* parent, OPimTodo *todo)
: ResultItem(parent)
{
	_todo = todo;
	setText( 0, todo->toShortText() );
	setIcon();
}

TodoItem::~TodoItem()
{
	delete _todo;
}

QString TodoItem::toRichText()
{
	return _todo->toRichText();
}

void TodoItem::action( int act )
{
	if (act == 0){
		QCopEnvelope e("QPE/Application/todolist", "show(int)");
		e << _todo->uid();
	}else if (act == 1){
		QCopEnvelope e("QPE/Application/todolist", "edit(int)");
		e << _todo->uid();
	}
}

QIntDict<QString> TodoItem::actions()
{
	QIntDict<QString> result;
	result.insert( 0, new QString( QObject::tr("show") ) );
	result.insert( 1, new QString( QObject::tr("edit") ) );
	return result;
}

void TodoItem::setIcon()
{
	QPixmap icon;
	switch ( _todo->lastHitField() ) {
	case -1:
		icon = Resource::loadPixmap( "reset" );
		break;
	case OPimTodo::Description:
		icon = Resource::loadPixmap( "txt" );
		break;
 	case OPimTodo::Summary:
		icon = Resource::loadPixmap( "osearch/summary" );
 		break;
	case OPimTodo::Priority:
		icon = Resource::loadPixmap( "todo/priority1" );
 		break;
 	case OPimTodo::HasDate:
		icon = Resource::loadPixmap( "osearch/clock" );
 		break;
	default:
		icon = Resource::loadPixmap( "DocsIcon" );
		break;
	}
	setPixmap( 0, icon );
}
