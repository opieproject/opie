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
#ifndef TODOITEM_H
#define TODOITEM_H


#include <opie2/opimtodo.h>
#include "resultitem.h"


/**
@author Patrick S. Vogt
*/
class TodoItem : public ResultItem
{
public:
    TodoItem(OListViewItem* parent, Opie::OPimTodo *todo);
    ~TodoItem();

    virtual QString toRichText();
    virtual void action( int );
    virtual QIntDict<QString> actions();

private:
    void setIcon();
    Opie::OPimTodo *_todo;

};

#endif
