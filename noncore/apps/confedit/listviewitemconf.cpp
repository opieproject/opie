/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "listviewitemconf.h"

ListViewItemConf::ListViewItemConf(ListViewItemConf *parent)
	: QListViewItem(parent), _changed(false)
{
}
	
ListViewItemConf::ListViewItemConf(QListView *parent)
	: QListViewItem(parent), _changed(false)
{
	_type = File;
}
	
ListViewItemConf::~ListViewItemConf()
{
}

int ListViewItemConf::getType()
{
	return _type;
}

void ListViewItemConf::changed()
{
	_changed=true;
 	displayText();
}

void ListViewItemConf::unchanged()
{
	_changed=false;
 	for (QListViewItem *it = firstChild(); it!=0;it = it->nextSibling())
  {
   	((ListViewItemConf*)it)->unchanged();
  }
 	displayText();
}