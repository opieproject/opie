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

#include <opie2/oresource.h>

#include <qpe/qcopenvelope_qws.h>


using namespace Opie;
EventItem::EventItem(OListViewItem* parent, OPimEvent *event)
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
        icon = Opie::Core::OResource::loadPixmap( "reset", Opie::Core::OResource::SmallIcon );
//		QWhatsThis::add( icon, QObject::tr("Enter your search terms here") );
		break;
	case Qtopia::DatebookDescription:
        icon = Opie::Core::OResource::loadPixmap( "osearch/summary", Opie::Core::OResource::SmallIcon );
 		break;
 	case Qtopia::Notes:
        icon = Opie::Core::OResource::loadPixmap( "txt", Opie::Core::OResource::SmallIcon );
 		break;
	case Qtopia::Location:
        icon = Opie::Core::OResource::loadPixmap( "home", Opie::Core::OResource::SmallIcon );
 		break;
 	case Qtopia::StartDateTime:
 	case Qtopia::EndDateTime:
        icon = Opie::Core::OResource::loadPixmap( "osearch/clock", Opie::Core::OResource::SmallIcon );
 		break;
	default:
        icon = Opie::Core::OResource::loadPixmap( "DocsIcon", Opie::Core::OResource::SmallIcon );
		break;
	}
	setPixmap( 0, icon );
}
