/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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
#include "browsekeyentry.h"

#include <qtoolbutton.h>
#include <qwidgetstack.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>
#include <qhbox.h>
#include <qdatetime.h>

//#include <iostream.h>
#include <qheader.h>
// For qWarning(const char *)

/*!
    \class TVBrowseKeyEntry
    \brief a Widget used enter keys into the TVBrowseViewWidget

    The TVBrowseKeyEntry Widget provides the facility to enter
    various key types to be search on in the table.  The key can be changed
    and the entry field will update to the correct sort of widget appropriately
*/

/*!
    Constructs the widget 
*/
TVBrowseKeyEntry::TVBrowseKeyEntry(QWidget *parent, const char *name, WFlags f)
    : QWidget(parent, name, f)
{
    int stack_elem = 0;
    QHBoxLayout *h_layout = new QHBoxLayout(this);

    textKey = new QLineEdit(this, 0);

    dateKey = new QHBox(this, 0);
    dayKey = new QLineEdit(dateKey, 0);
    monthKey = new QLineEdit(dateKey, 0);
    yearKey = new QLineEdit(dateKey, 0);

    timeKey = new QHBox(this, 0);
    hourKey = new QLineEdit(timeKey, 0);
    minuteKey = new QLineEdit(timeKey, 0);
    secondKey = new QLineEdit(timeKey, 0);

    resetButton = new QPushButton(this, "reset");
    resetButton->setMinimumSize(QSize(50, 0));
    resetButton->setText(tr("Reset"));

    changeKeyButton = new QToolButton(this, "changekey");
    // TODO The icon stuff.
    changeKeyButton->setText(tr("key"));

    totalKeys = 0;
    ts = 0;
    keyMenu = new QPopupMenu(this, "keymenu");

    ws = new QWidgetStack(this, 0);
    ws->addWidget(textKey, stack_elem++);
    ws->addWidget(timeKey, stack_elem++);
    ws->addWidget(dateKey, stack_elem++);

    ws->raiseWidget(0);
   
    // TODO connect slots and signals....
    connect(changeKeyButton, SIGNAL(clicked()), 
            this, SLOT(changeKeyMenuSlot()));
    connect(resetButton, SIGNAL(clicked()), 
            this, SLOT(resetKeySlot()));

    connect(textKey, SIGNAL(textChanged(const QString&)), 
            this, SLOT(searchOnText()));

    connect(dayKey, SIGNAL(textChanged(const QString&)), 
            this, SLOT(searchOnText()));
    connect(monthKey, SIGNAL(textChanged(const QString&)), 
            this, SLOT(searchOnText()));
    connect(yearKey, SIGNAL(textChanged(const QString&)), 
            this, SLOT(searchOnText()));

    connect(secondKey, SIGNAL(textChanged(const QString&)), 
            this, SLOT(searchOnText()));
    connect(minuteKey, SIGNAL(textChanged(const QString&)), 
            this, SLOT(searchOnText()));
    connect(hourKey, SIGNAL(textChanged(const QString&)), 
            this, SLOT(searchOnText()));
 
    h_layout->addWidget(ws);
    h_layout->addWidget(resetButton);
    h_layout->addWidget(changeKeyButton);
}

/*!
    Destructs the widget 
*/
TVBrowseKeyEntry::~TVBrowseKeyEntry()
{
}

/*!
    Changes which key the user intends to search on

    \param id_param the index of the key future searches should be base on
*/
void TVBrowseKeyEntry::changeKeySlot(int id_param)
{
    emit sortChanged(id_param);
    switch(ts->kRep->getKeyType(ts->current_column)) {
        /* work out what to raise */
        case kt_string:
        case kt_int:
            ws->raiseWidget(0);
            break;
        case kt_time:
            ws->raiseWidget(1);
            break;
        case kt_date:
            ws->raiseWidget(2);
            break;
        default: 
            return; 
    }
}

/*!
    Opens the change key menu
*/
void TVBrowseKeyEntry::changeKeyMenuSlot()
{
    if(ts) 
        keyMenu->exec(changeKeyButton->mapToGlobal(QPoint(0,0)));
}
/*!
    Blanks the key entry widget
    \TODO the actual implmentation
*/
void TVBrowseKeyEntry::resetKeySlot() {
    ;
}

void TVBrowseKeyEntry::setTableState(TableState *t) {
    int i;
    ts = t;

    /* clear the old */
    keyMenu->clear();

    for (i = 0; i < t->kRep->getNumFields(); i++) {
        keyMenu->insertItem(ts->kRep->getKeyName(i), this, 
                SLOT(changeKeySlot(int)), 0, i);
        keyMenu->setItemParameter(i, i);
    }
}

/*!
    Searches on the current value of the key entry provided that the 
    current key is of type text WARNING, TODO fix memory leaks 
*/
void TVBrowseKeyEntry::searchOnText()
{
    void *sendkey;
    int tmp;

    switch(ts->kRep->getKeyType(ts->current_column)) {
        /* work out what to raise */
        case kt_string:
            sendkey = (void *)new QString(textKey->text());
            break;
        case kt_int: {
            bool ok;
            tmp = textKey->text().toInt(&ok);
            sendkey = &tmp;
            if (!ok)
                return;
            break;
        }
        case kt_time: {
            bool ok;
            int s,m,h;
            s = secondKey->text().toInt(&ok);
            if (!ok)
                return;
            m = minuteKey->text().toInt(&ok);
            if (!ok)
                return;
            h = hourKey->text().toInt(&ok);
            if (!ok)
                return;
            if(!QTime::isValid(h, m, s))
                return;
            sendkey = (void *) new QTime(h, m, s);
            break;
        }
        case kt_date: {
            bool ok;
            int d,m,y;
            d = dayKey->text().toInt(&ok);
            if (!ok)
                return;
            m = monthKey->text().toInt(&ok);
            if (!ok)
                return;
            y = yearKey->text().toInt(&ok);
            if (!ok)
                return;
            if(!QDate::isValid(y, m, d))
                return;
            sendkey = (void *) new QDate(y, m, d);
            break;
        }
        default:
            qWarning("TVBrowseKeyEntry::searchOnText() "
                     "cannot work out data type");
            return;
    }
    emit searchOnKey(ts->current_column, sendkey);
}

/*! \fn void TVBrowseKeyEntry::searchOnKey(int currentKeyId, void *v)

    This signal indicates that a search on key index currentKeyId should be
    done searching for the value v.
*/ 
