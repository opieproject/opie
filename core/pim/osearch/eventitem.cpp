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

#include <qpe/qcopenvelope_qws.h>
#include <opie/oevent.h>

EventItem::EventItem(OListViewItem* parent, OEvent *event)
 : ResultItem(parent)
{
	_event = event;
	setText(0, _event->description() );
}


EventItem::~EventItem()
{
}


QString EventItem::toRichText()
{
    return _event->toRichText();
}

void EventItem::editItem()
{
	QCopEnvelope e("QPE/Application/datebook", "editEvent(int)");
	e << _event->uid();
}