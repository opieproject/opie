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
#ifndef RESULTITEM_H
#define RESULTITEM_H

#include <qintdict.h>

#include "olistviewitem.h"

/**
@author Patrick S. Vogt
*/
class ResultItem : public OListViewItem
{
public:
    ResultItem(OListViewItem* parent);
    ~ResultItem();

    virtual QString toRichText() {return "no text";};
    virtual int rtti() { return Result;}
    virtual void action( int ) = 0;
    virtual QIntDict<QString> actions() = 0;
};

#endif
