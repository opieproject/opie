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

#include <opie/ocontact.h>
#include <qpe/qcopenvelope_qws.h>

ContactItem::ContactItem(OListViewItem* parent, OContact *contact)
: ResultItem(parent)
{
	_contact = contact;
	setText(0, _contact->toShortText());
}


ContactItem::~ContactItem()
{
	delete _contact;
}


QString ContactItem::toRichText()
{
    return _contact->toRichText();
}


void ContactItem::editItem()
{
	QCopEnvelope e("QPE/Application/addressbook", "edit(int)");
//	QCopEnvelope e("QPE/Addressbook", "editEvent(int)");
	e << _contact->uid();
}

void ContactItem::showItem()
{
	QCopEnvelope e("QPE/Application/addressbook", "show(int)");
//	QCopEnvelope e("QPE/Addressbook", "viewDefault(QDate)");
	e << _contact->uid();
}

