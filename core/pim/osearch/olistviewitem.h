/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LISTVIEWITEMCONF_H
#define LISTVIEWITEMCONF_H

#include <qlistview.h>


class OListViewItem : public QListViewItem
{
public:
	enum {Raw, Searchgroup, Result, Contact, Todo, Event};
	OListViewItem(OListViewItem *parent);
	OListViewItem(OListViewItem *parent, QString name);
	OListViewItem(QListView *parent, QString name);
	~OListViewItem();

  	virtual void expand(){};
	virtual int rtti() { return Raw;}
};

#endif
