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
  void save();
  virtual void revert() = 0;
  virtual void remove() = 0;
  virtual void displayText() = 0;
  virtual void expand() = 0;
  virtual void changed();
  bool isChanged() {return _changed;};
  virtual void unchanged();
  virtual bool revertable();

protected:
	int _type;
 	bool _changed;
};

#endif
