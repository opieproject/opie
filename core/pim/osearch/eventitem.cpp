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
#include <qpixmap.h>
#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>
//#include <qwhatsthis.h>
#include <opie/oevent.h>

EventItem::EventItem(OListViewItem* parent, OEvent *event)
 : ResultItem(parent)
{
	_event = event;
	setText(0, _event->toShortText() );
	setIcon();
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

void EventItem::setIcon()
{
	QPixmap icon;
	switch ( _event->lastHitField() ) {
	case -1:
		icon = Resource::loadPixmap( "reset" );
//		QWhatsThis::add( icon, QObject::tr("Enter your search terms here") );
		break;
	case Qtopia::DatebookDescription:
		icon = Resource::loadPixmap( "osearch/summary" );
 		break;
 	case Qtopia::Notes:
		icon = Resource::loadPixmap( "txt" );
 		break;
	case Qtopia::Location:
		icon = Resource::loadPixmap( "home" );
 		break;
 	case Qtopia::StartDateTime:
 	case Qtopia::EndDateTime:
		icon = Resource::loadPixmap( "osearch/clock" );
 		break;
	default:
		icon = Resource::loadPixmap( "DocsIcon" );
		break;
	}
	setPixmap( 0, icon );
}
