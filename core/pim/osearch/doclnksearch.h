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

/**
@author Patrick S. Vogt
*/
class DocLnkSearch : public AppLnkSearch 
{
public:
    DocLnkSearch(QListView* parent, QString name);
    ~DocLnkSearch();

protected:
	virtual void load();
	virtual void insertItem( void* );

};

#endif
