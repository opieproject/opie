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
#ifndef DOCLNKITEM_H
#define DOCLNKITEM_H

#include "resultitem.h"

class DocLnk;

/**
@author Patrick S. Vogt
*/
class DocLnkItem : public ResultItem
{
public:
    DocLnkItem(OListViewItem* parent, DocLnk *app);
    ~DocLnkItem();

    virtual QString toRichText();
    virtual void editItem();
    virtual void showItem();

private:
    DocLnk *_doc;
};

#endif
