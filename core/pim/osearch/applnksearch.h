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
#ifndef APPLNKSEARCH_H
#define APPLNKSEARCH_H

#include "searchgroup.h"

class AppLnkSet;

/**
@author Patrick S. Vogt
*/
class AppLnkSearch : public SearchGroup
{
public:
    AppLnkSearch(QListView* parent, QString name);

    ~AppLnkSearch();

protected:
	virtual void load();
	virtual int search();
	virtual void insertItem( void* );

	AppLnkSet *_apps;
};

#endif
