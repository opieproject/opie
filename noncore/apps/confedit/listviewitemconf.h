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


class ListViewItemConf : public QListViewItem
{
public:
	enum {File, Group, Key};

	ListViewItemConf(ListViewItemConf *parent);
	ListViewItemConf(QListView *parent);
	~ListViewItemConf();

 	int getType();
  virtual void displayText() = 0;
  virtual void changed();
  virtual void unchanged();

protected:
	int _type;
 	bool _changed;
};

#endif
