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
#ifndef SEARCHGROUP_H
#define SEARCHGROUP_H

#include "olistviewitem.h"

#include <qregexp.h>

//#define LIPBOPIE_SEARCH

/**
@author Patrick S. Vogt
*/
class SearchGroup : public OListViewItem
{
public:
    SearchGroup(QListView* parent, QString name);

    ~SearchGroup();

    virtual void expand();
    virtual void setSearch(QString);
    virtual void setSearch(QRegExp);
    virtual int rtti() { return Searchgroup;}

protected:
	QRegExp _search;
private:
	QString _name;
	bool expanded;
};

#endif
