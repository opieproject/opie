/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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
#include "commonwidgets.h"

#include <qtoolbutton.h>
#include <qwidgetstack.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>
#include <qhbox.h>
#include <qdatetime.h>

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
    QHBoxLayout *h_layout = new QHBoxLayout(this);

    textKey = new QLineEdit(this, 0);
    intKey = new IntEdit(this, 0);
    dateKey = new DateEdit(this, 0);
    timeKey = new TimeEdit(this, 0);

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
    ws->addWidget(textKey, TVVariant::String);
    ws->addWidget(intKey, TVVariant::Int);
    ws->addWidget(timeKey, TVVariant::Time);
    ws->addWidget(dateKey, TVVariant::Date);

    ws->raiseWidget(TVVariant::String);
   
    // TODO connect slots and signals....
    connect(changeKeyButton, SIGNAL(clicked()), 
            this, SLOT(changeKeyMenuSlot()));

    connect(resetButton, SIGNAL(clicked()), 
            textKey, SLOT(clear()));
    connect(resetButton, SIGNAL(clicked()), 
            intKey, SLOT(clear()));
    connect(resetButton, SIGNAL(clicked()), 
            dateKey, SLOT(clear()));
    connect(resetButton, SIGNAL(clicked()), 
            timeKey, SLOT(clear()));

    h_layout->addWidget(ws);
    h_layout->addWidget(resetButton);
    h_layout->addWidget(changeKeyButton);

    connect(textKey, SIGNAL(textChanged(const QString&)), 
            this, SLOT(searchOnText()));
    connect(intKey, SIGNAL(valueChanged(int)), 
            this, SLOT(searchOnText()));
    connect(dateKey, SIGNAL(valueChanged(const QDate&)), 
            this, SLOT(searchOnText()));
    connect(timeKey, SIGNAL(valueChanged(const QTime&)), 
            this, SLOT(searchOnText()));
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
    if(ts) {
        emit sortChanged(id_param);
        ws->raiseWidget(ts->kRep->getKeyType(ts->current_column));
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


void TVBrowseKeyEntry::setTableState(TableState *t) {
    ts = t;
}

void TVBrowseKeyEntry::rebuildKeys() {
    int i;
    if (!ts) return;
    if (!ts->kRep) return;

    /* clear the old */
    keyMenu->clear();

    KeyListIterator it(*ts->kRep);

    for (i = 0; i < ts->kRep->getNumFields(); i++) {
        keyMenu->insertItem(it.current()->name(), this, 
                SLOT(changeKeySlot(int)), 0, i);
        keyMenu->setItemParameter(i, it.currentKey());
        ++it;
    }
}

void TVBrowseKeyEntry::reset()
{
    textKey->clear();
    intKey->clear();
    dateKey->clear();
    timeKey->clear();

    keyMenu->clear();
}
/*!
    Searches on the current value of the key entry provided that the 
    current key is of type text WARNING, TODO fix memory leaks 
*/
void TVBrowseKeyEntry::searchOnText()
{
    TVVariant sendkey;

    if (!ts)
        return;

    switch(ts->kRep->getKeyType(ts->current_column)) {
        case TVVariant::String:
            sendkey = TVVariant(QString(textKey->text()));
            break;
        case TVVariant::Int: {
            sendkey = TVVariant(intKey->value());
            break;
        }
        case TVVariant::Time: {
            sendkey = TVVariant(QTime(timeKey->time()));
            break;
        }
        case TVVariant::Date: {
            sendkey = TVVariant(QDate(dateKey->date()));
            break;
        }
	case TVVariant::Invalid:
	    break;
        default:
            qWarning("TVBrowseKeyEntry::searchOnText() "
                     "cannot work out data type");
            return;
    }
    emit searchOnKey(ts->current_column, sendkey);
}

/*! \fn void TVBrowseKeyEntry::searchOnKey(int currentKeyId, TVVariant)

    This signal indicates that a search on key index currentKeyId should be
    done searching for the value v.
*/ 
