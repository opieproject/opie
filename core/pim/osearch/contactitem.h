//
//
// C++ Interface: $MODULE$
//
// Description:
//
//
// Author: Patrick S. Vogt <tille@handhelds.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONTACTITEM_H
#define CONTACTITEM_H

#include "resultitem.h"

class OContact;

/**
@author Patrick S. Vogt
*/
class ContactItem : public ResultItem
{
public:
    ContactItem(OListViewItem* parent, OContact *contact);

    ~ContactItem();

    virtual QString toRichText();

private:
	OContact *_contact;

};

#endif
