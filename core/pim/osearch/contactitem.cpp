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
#include "contactitem.h"

#include <qpixmap.h>
#include <opie/ocontact.h>
#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>

ContactItem::ContactItem(OListViewItem* parent, OContact *contact)
: ResultItem(parent)
{
	_contact = contact;
	setText(0, _contact->toShortText());
	setIcon();
}

void ContactItem::setIcon()
{
	QPixmap icon;
	switch ( _contact->lastHitField() ) {
	case -1:
		icon = Resource::loadPixmap( "reset" );
		break;
	case Qtopia::BusinessPhone:
		icon = Resource::loadPixmap( "addressbook/phonework" );
		break;
	case Qtopia::BusinessFax:
		icon = Resource::loadPixmap( "addressbook/faxwork" );
		break;
	case Qtopia::BusinessMobile:
		icon = Resource::loadPixmap( "addressbook/mobilework" );
		break;
	case Qtopia::DefaultEmail:
		icon = Resource::loadPixmap( "addressbook/email" );
		break;
	case Qtopia::Emails:
		icon = Resource::loadPixmap( "addressbook/email" );
		break;
	case Qtopia::HomePhone:
		qDebug("homephone");
		icon = Resource::loadPixmap( "addressbook/phonehome" );
		break;
	case Qtopia::HomeFax:
		icon = Resource::loadPixmap( "addressbook/faxhome" );
		break;
	case Qtopia::HomeMobile:
		icon = Resource::loadPixmap( "addressbook/mobilehome" );
		break;
	case Qtopia::HomeWebPage:
		icon = Resource::loadPixmap( "addressbook/webpagehome" );
		break;
	case Qtopia::BusinessWebPage:
		icon = Resource::loadPixmap( "addressbook/webpagework" );
		break;
	case Qtopia::Title:
	case Qtopia::FirstName:
	case Qtopia::MiddleName:
	case Qtopia::LastName:
	case Qtopia::Suffix:
	case Qtopia::Nickname:
 	case Qtopia::FileAs:
		icon = Resource::loadPixmap( "addressbook/identity" );
 		break;
 	case Qtopia::HomeStreet:
 	case Qtopia::HomeCity:
 	case Qtopia::HomeState:
 	case Qtopia::HomeZip:
 	case Qtopia::HomeCountry:
		icon = Resource::loadPixmap( "addressbook/addresshome" );
 		break;
 	case Qtopia::Company:
 	case Qtopia::BusinessCity:
 	case Qtopia::BusinessStreet:
 	case Qtopia::BusinessZip:
 	case Qtopia::BusinessCountry:
 	case Qtopia::JobTitle:
 	case Qtopia::Department:
 	case Qtopia::Office:
 	case Qtopia::Manager:
 	case Qtopia::BusinessPager:
 	case Qtopia::Profession:
		icon = Resource::loadPixmap( "addressbook/addresshome" );
 		break;
 	case Qtopia::Assistant:
	case Qtopia::Spouse:
 	case Qtopia::Children:
		icon = Resource::loadPixmap( "osearch/personal" );
 		break;
 	case Qtopia::Birthday:
 	case Qtopia::Anniversary:
		icon = Resource::loadPixmap( "osearch/clock" );
 		break;
 	case Qtopia::Notes:
 		break;
	default:
		icon = Resource::loadPixmap( "DocsIcon" );
		break;
	}
	setPixmap( 0, icon );
}

ContactItem::~ContactItem()
{
	delete _contact;
}


QString ContactItem::toRichText()
{
    return _contact->toRichText();
}

void ContactItem::action( int act )
{
if (act == 0){
	QCopEnvelope e("QPE/Application/addressbook", "show(int)");
	e << _contact->uid();
}else if(act == 1){
 	QCopEnvelope e("QPE/Application/addressbook", "edit(int)");
 	e << _contact->uid();
}

}

QIntDict<QString> ContactItem::actions()
{
	QIntDict<QString> result;
	result.insert( 0, new QString( QObject::tr("show") ) );
	result.insert( 1, new QString( QObject::tr("edit") ) );
	return result;

}
