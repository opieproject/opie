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

#include <opie2/oresource.h>

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
    if (act == 0) {
        QCopEnvelope e("QPE/Application/todolist", "show(int)");
        e << _todo->uid();
    }
    else if (act == 1) {
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
            icon = Opie::Core::OResource::loadPixmap( "reset", Opie::Core::OResource::SmallIcon );
            break;
        case OPimTodo::Description:
            icon = Opie::Core::OResource::loadPixmap( "txt", Opie::Core::OResource::SmallIcon );
            break;
        case OPimTodo::Summary:
            icon = Opie::Core::OResource::loadPixmap( "osearch/summary", Opie::Core::OResource::SmallIcon );
            break;
        case OPimTodo::Priority:
            icon = Opie::Core::OResource::loadPixmap( "todo/priority1", Opie::Core::OResource::SmallIcon );
            break;
        case OPimTodo::HasDate:
            icon = Opie::Core::OResource::loadPixmap( "osearch/clock", Opie::Core::OResource::SmallIcon );
            break;
        default:
            icon = Opie::Core::OResource::loadPixmap( "DocsIcon", Opie::Core::OResource::SmallIcon );
            break;
    }
    setPixmap( 0, icon );
}
