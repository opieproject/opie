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
#ifndef DATEBOOKSEARCH_H
#define DATEBOOKSEARCH_H

#include "searchgroup.h"

class ODateBookAccess;
class QAction;

/**
@author Patrick S. Vogt
*/
class DatebookSearch : public SearchGroup
{
public:
    DatebookSearch(QListView* parent, QString name);
    ~DatebookSearch();

    virtual QPopupMenu* popupMenu();

protected:
	virtual void load();
	virtual int search();
	virtual void insertItem( void* );

private:
    ODateBookAccess *_dates;
    QAction *actionShowPastEvents;
    QAction *actionSearchInDates;
    QPopupMenu *_popupMenu;
};

#endif
