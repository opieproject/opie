/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
  // (c) 2002 Patrick S. Vogtp <tille@handhelds.org>

#include "olistview.h"
#include "olistviewitem.h"
#include <qmessagebox.h>


OListView::OListView(QWidget *parent, const char *name )
  : QListView(parent,name)
{

	setRootIsDecorated( true );
	addColumn(tr("Results"));
	setSorting( -1 );
 	connect( this, SIGNAL(expanded(QListViewItem*)), SLOT(expand(QListViewItem*)));
}


OListView::~OListView()
{
}

void OListView::expand(QListViewItem *item)
{
 	((OListViewItem*)item)->expand();
}

