/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ochecklistitem.h"

OCheckListItem::OCheckListItem(OCheckListItem *parent, const char* name)
	: QCheckListItem(parent,name)
{
	dummy = new QCheckListItem(this,"dummy");
}
	
OCheckListItem::OCheckListItem(QListView *parent, const char* name)
	: QCheckListItem(parent,name)
{
	dummy = new QCheckListItem(this,"dummy");
}
	
OCheckListItem::~OCheckListItem()
{
}

void OCheckListItem::expand()
{
 	if (dummy!=0) delete dummy;
  dummy = 0;
}