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
#include "eventitem.h"

#include <qdatetime.h>
#include <qpe/qcopenvelope_qws.h>
#include <opie/oevent.h>

EventItem::EventItem(OListViewItem* parent, OEvent *event)
 : ResultItem(parent)
{
	_event = event;
	setText(0, _event->toShortText() );
}


EventItem::~EventItem()
{
}


QString EventItem::toRichText()
{
    return _event->toRichText();
}

void EventItem::action( int act )
{
	if (act == 0){
		QCopEnvelope e("QPE/Application/datebook", "viewDefault(QDate)");
		e << _event->startDateTime().date();
	}else if(act == 1){
 		QCopEnvelope e("QPE/Application/datebook", "editEvent(int)");
 		e << _event->uid();
	}
}

QIntDict<QString> EventItem::actions()
{
	QIntDict<QString> result;
	result.insert( 0, new QString( QObject::tr("show") ) );
	result.insert( 1, new QString( QObject::tr("edit") ) );
	return result;
}
