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

#include <opie2/opimrecord.h>
#include <opie2/opimcontact.h>

#include "resultitem.h"


/**
@author Patrick S. Vogt
*/
class ContactItem : public ResultItem
{
public:
    ContactItem(OListViewItem* parent, Opie::OPimContact *contact);

    ~ContactItem();

    virtual QString toRichText();
    virtual void action( int );
    virtual QIntDict<QString> actions();


private:
	void setIcon();
    Opie::OPimContact *_contact;

};

#endif
