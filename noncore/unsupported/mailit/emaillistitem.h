/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef EMAILLISTITEM_H
#define EMAILLISTITEM_H

#include <qlistview.h>
#include "emailhandler.h"

class EmailListItem: public QListViewItem
{
public:
	EmailListItem(QListView *parent, Email mailIn, bool inbox);
	Email* getMail();
	void setMail(Email newMail);
	void setItemSelected(bool enable);
	bool isItemSelected();
	bool itemSelected();

protected:
	void paintCell( QPainter *p, const QColorGroup &cg,
			int column, int width, int alignment );
private:
	Email mail;
	bool selected;
	QString dateFromULCString( QString ulc );
};

#endif
