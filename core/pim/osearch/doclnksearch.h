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
#ifndef DOCLNKSEARCH_H
#define DOCLNKSEARCH_H

#include "applnksearch.h"

class QAction;
class QPopupMenu;

/**
@author Patrick S. Vogt
*/
class DocLnkSearch : public AppLnkSearch
{
public:
    DocLnkSearch(QListView* parent, QString name);
    ~DocLnkSearch();

    virtual QPopupMenu* popupMenu();

protected:
	virtual void load();
	virtual bool searchFile(AppLnk*);
	virtual void insertItem( void* );
private:
	QAction *actionSearchInFiles;
	QPopupMenu *_popupMenu;

};

#endif
