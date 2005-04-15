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

#include <opie2/oresource.h>

#include <qpe/qcopenvelope_qws.h>


using namespace Opie;
ContactItem::ContactItem(OListViewItem* parent, OPimContact *contact)
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
		icon = Opie::Core::OResource::loadPixmap( "reset", Opie::Core::OResource::SmallIcon );
		break;
	case Qtopia::BusinessPhone:
        icon = Opie::Core::OResource::loadPixmap( "addressbook/phonework", Opie::Core::OResource::SmallIcon );
		break;
	case Qtopia::BusinessFax:
        icon = Opie::Core::OResource::loadPixmap( "addressbook/faxwork", Opie::Core::OResource::SmallIcon );
		break;
	case Qtopia::BusinessMobile:
        icon = Opie::Core::OResource::loadPixmap( "addressbook/mobilework", Opie::Core::OResource::SmallIcon );
		break;
	case Qtopia::DefaultEmail:
	case Qtopia::Emails:
        icon = Opie::Core::OResource::loadPixmap( "addressbook/email", Opie::Core::OResource::SmallIcon );
		break;
	case Qtopia::HomePhone:
        icon = Opie::Core::OResource::loadPixmap( "addressbook/phonehome", Opie::Core::OResource::SmallIcon );
		break;
	case Qtopia::HomeFax:
        icon = Opie::Core::OResource::loadPixmap( "addressbook/faxhome", Opie::Core::OResource::SmallIcon );
		break;
	case Qtopia::HomeMobile:
        icon = Opie::Core::OResource::loadPixmap( "addressbook/mobilehome", Opie::Core::OResource::SmallIcon );
		break;
	case Qtopia::HomeWebPage:
        icon = Opie::Core::OResource::loadPixmap( "addressbook/webpagehome", Opie::Core::OResource::SmallIcon );
		break;
	case Qtopia::BusinessWebPage:
        icon = Opie::Core::OResource::loadPixmap( "addressbook/webpagework", Opie::Core::OResource::SmallIcon );
		break;
	case Qtopia::Title:
 	case Qtopia::JobTitle:
	case Qtopia::FirstName:
	case Qtopia::MiddleName:
	case Qtopia::LastName:
	case Qtopia::Suffix:
	case Qtopia::Nickname:
 	case Qtopia::FileAs:
        icon = Opie::Core::OResource::loadPixmap( "addressbook/identity", Opie::Core::OResource::SmallIcon );
 		break;
 	case Qtopia::HomeStreet:
 	case Qtopia::HomeCity:
 	case Qtopia::HomeState:
 	case Qtopia::HomeZip:
 	case Qtopia::HomeCountry:
        icon = Opie::Core::OResource::loadPixmap( "osearch/addresshome", Opie::Core::OResource::SmallIcon );
 		break;
 	case Qtopia::Company:
 	case Qtopia::BusinessCity:
 	case Qtopia::BusinessStreet:
 	case Qtopia::BusinessZip:
 	case Qtopia::BusinessCountry:
 	case Qtopia::Department:
 	case Qtopia::Office:
 	case Qtopia::Manager:
 	case Qtopia::BusinessPager:
 	case Qtopia::Profession:
        icon = Opie::Core::OResource::loadPixmap( "osearch/addresshome", Opie::Core::OResource::SmallIcon );
 		break;
 	case Qtopia::Assistant:
	case Qtopia::Spouse:
 	case Qtopia::Children:
        icon = Opie::Core::OResource::loadPixmap( "osearch/personal", Opie::Core::OResource::SmallIcon );
 		break;
 	case Qtopia::Birthday:
 	case Qtopia::Anniversary:
        icon = Opie::Core::OResource::loadPixmap( "osearch/clock", Opie::Core::OResource::SmallIcon );
 		break;
 	case Qtopia::Notes:
        icon = Opie::Core::OResource::loadPixmap( "txt", Opie::Core::OResource::SmallIcon );
 		break;
	default:
        icon = Opie::Core::OResource::loadPixmap( "DocsIcon", Opie::Core::OResource::SmallIcon );
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
