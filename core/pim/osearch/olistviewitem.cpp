/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "olistviewitem.h"

OListViewItem::OListViewItem(OListViewItem *parent)
	: QListViewItem(parent)
{
}

OListViewItem::OListViewItem(OListViewItem *parent, QString name)
	: QListViewItem(parent,name)
{
}

OListViewItem::OListViewItem(QListView *parent, QString name)
	: QListViewItem(parent, name)
{
}

OListViewItem::~OListViewItem()
{
}

