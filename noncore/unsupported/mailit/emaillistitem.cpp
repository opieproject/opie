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
  // setText(2,mail.date);
  setText(2,dateFromULCString(mail.date));

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

/*  if (selected) {
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

/*
 * Converts an E-Mail date (ULC) RFC 2822 conform to a QDateTime.
 * Returning a QString with formatting of "YYYY-MM-DD HH:MM:SS"
 * (zodiac: This method was tested with more than 300 inbox mails,
 * it didn't slow down the loading of mail-it.)
 */
QString EmailListItem::dateFromULCString( QString ulcDate )
{
	QString sTemp, sTime;
	int iPos, iDay, iMon=1, iYear;

	iPos=ulcDate.find(',');
	if (iPos) { // it has a day-of-week
		ulcDate=ulcDate.remove(0,++iPos); //.stripWhiteSpace();
	}

	QStringList dateEntries = QStringList::split(" ",ulcDate,FALSE);
	QStringList::Iterator iter = dateEntries.begin();

	// Get day as DD
	iDay = (*iter++).toInt();

	// Get month as string Mmm
	sTemp = (*iter++);
	if (sTemp =="Jan") {iMon=1;} else
	if (sTemp =="Feb") {iMon=2;} else
	if (sTemp =="Mar") {iMon=3;} else
	if (sTemp =="Apr") {iMon=4;} else
	if (sTemp =="May") {iMon=5;} else
	if (sTemp =="Jun") {iMon=6;} else
	if (sTemp =="Jul") {iMon=7;} else
	if (sTemp =="Aug") {iMon=8;} else
	if (sTemp =="Sep") {iMon=9;} else
	if (sTemp =="Oct") {iMon=10;} else
	if (sTemp =="Nov") {iMon=11;} else
	if (sTemp =="Dec") {iMon=12;}

	// Get year as YYYY or YY
	iYear = (*iter++).toInt();

	QDate date = QDate(iYear, iMon, iDay);

	// Convert timestring into a QTime
	QStringList timeEntries = QStringList::split(":",(*iter++),FALSE);
	QStringList::Iterator iterTime = timeEntries.begin();
	iYear=(*iterTime++).toInt(); // var reuse.. *cough*
	iMon=(*iterTime++).toInt();
	iDay=(*iterTime++).toInt();
	QTime time = QTime(iYear,iMon,iDay);

	return QString::number(date.year())+"-"
		+QString::number(date.month()).rightJustify(2,'0')+"-"
		+QString::number(date.day()).rightJustify(2,'0')+" "
		+time.toString();
}


