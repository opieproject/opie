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
#include <qstring.h>
#include <qpe/resource.h>
#include "emaillistitem.h"

EmailListItem::EmailListItem(QListView *parent, Email mailIn, bool inbox)
		: QListViewItem(parent)
{
	QString temp;
	
	mail = mailIn;
	
	if (inbox) {
		setText(0, mail.from);
	} else {
		QStringList::Iterator it = mail.recipients.begin();
		temp = *it;
		if (mail.recipients.count() > 1)
			temp += "...";
		setText(0, temp);
	}
	setText(1, mail.subject);
	setText(2,mail.date);
	
	if (mailIn.files.count()>0)
	{
		setPixmap(0, Resource::loadPixmap("mailit/attach"));
	}
	
	selected = FALSE;
}

Email* EmailListItem::getMail()
{
	return &mail;
}

void EmailListItem::setMail(Email newMail)
{
	mail = newMail;
	repaint();
}

void EmailListItem::setItemSelected(bool enable)
{
	selected = enable;
	setSelected(enable);
	repaint();
}

bool EmailListItem::isItemSelected()
{
	return selected;
}

void EmailListItem::paintCell( QPainter *p, const QColorGroup &cg,
				int column, int width, int alignment )
{

	QColorGroup _cg( cg );
	QColor c = _cg.text();

	if ( (! mail.read) && (mail.received) )
		_cg.setColor( QColorGroup::Text, Qt::blue);
	if (!mail.downloaded)
		_cg.setColor( QColorGroup::Text, Qt::red);
	
/*	if (selected) {
		_cg.setColor(QColorGroup::Base, Qt::blue);
		_cg.setColor(QColorGroup::Text, Qt::yellow);
		if (isSelected()) {
			_cg.setColor(QColorGroup::HighlightedText, Qt::yellow);
		} else {
			_cg.setColor(QColorGroup::Highlight, Qt::blue);
		}
	}
*/		
    QListViewItem::paintCell( p, _cg, column, width, alignment );

    _cg.setColor( QColorGroup::Text, c );
}
