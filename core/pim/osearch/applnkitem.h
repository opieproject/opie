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
#ifndef APPLNKITEM_H
#define APPLNKITEM_H

#include "resultitem.h"

class AppLnk;

/**
@author Patrick S. Vogt
*/
class AppLnkItem : public ResultItem
{
public:
    AppLnkItem(OListViewItem* parent, AppLnk *app);
    ~AppLnkItem();

    virtual QString toRichText();
    virtual void action( int );
    virtual QIntDict<QString> actions();

private:
    AppLnk *_app;
};

#endif
