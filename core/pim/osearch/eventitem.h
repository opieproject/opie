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
#ifndef EVENTITEM_H
#define EVENTITEM_H

#include "resultitem.h"

class OEvent;

/**
@author Patrick S. Vogt
*/
class EventItem : public ResultItem
{
public:
    EventItem(OListViewItem* parent, OEvent *event);

    ~EventItem();

    virtual QString toRichText();
    virtual void editItem();
    virtual void showItem();

private:
    OEvent *_event;

};

#endif
