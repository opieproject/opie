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

#include <opie2/opimevent.h>

using namespace Opie;

/**
@author Patrick S. Vogt
*/
class EventItem : public ResultItem
{
public:
    EventItem(OListViewItem* parent, OPimEvent *event);

    ~EventItem();

    virtual QString toRichText();
    virtual void action( int );
    virtual QIntDict<QString> actions();

private:
    void setIcon();
    OPimEvent *_event;

};

#endif
