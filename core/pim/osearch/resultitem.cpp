//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Patrick S. Vogt <tille@handhelds.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "resultitem.h"

#include <qmessagebox.h>

#include <opie/otodo.h>


ResultItem::ResultItem(OListViewItem* parent)
: OListViewItem(parent)
{

}


ResultItem::~ResultItem()
{
}

void ResultItem::showItem()
{
	QMessageBox::warning(0,"Not implemented","show is not yet implemented for this");
}


void ResultItem::editItem()
{
	QMessageBox::warning(0,"Not implemented","edit is not yet implemented for this");
}

