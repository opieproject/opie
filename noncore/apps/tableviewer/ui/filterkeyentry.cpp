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
#include "filterkeyentry.h"
#include "commonwidgets.h"

#include <qwidgetstack.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qsizepolicy.h>
#include <qdatetime.h>
#include <qhbox.h>

TVFilterKeyEntry::TVFilterKeyEntry(QWidget *parent, const char *name, WFlags f)
    : QWidget(parent, name, f)
{
    int stack_elem = 0;

    layout = new QHBoxLayout(this, 0);
    layout->setSpacing(0);
    layout->setMargin(0);

    textEntry = new QHBox(this, 0);
    textEntry->setSpacing(0);
    textEntry->setMargin(0);

    intEntry = new QHBox(this, 0);
    intEntry->setSpacing(0);
    intEntry->setMargin(0);

    timeEntry = new QHBox(this, 0);
    timeEntry->setSpacing(0);
    timeEntry->setMargin(0);

    dateEntry = new QHBox(this, 0);
    dateEntry->setSpacing(0);
    dateEntry->setMargin(0);

    textCombo = new QComboBox(textEntry, 0);
    textKey = new QLineEdit(textEntry, 0);

    /* Build the text combo list */
    textCombo->insertItem("less than");
    textCombo->insertItem("more than");
    textCombo->insertItem("equal to");
    textCombo->insertItem("containing");
    textCombo->insertItem("starting with");
    textCombo->insertItem("ending with");

    intCombo = new QComboBox(intEntry, 0);
    intKey = new IntEdit(intEntry, 0);

    /* Build the int combo list */
    intCombo->insertItem("less than");
    intCombo->insertItem("more than");
    intCombo->insertItem("equal to");

    timeCombo = new QComboBox(timeEntry, 0);
    timeKey = new TimeEdit(timeEntry, 0);

    /* Build the time combo list */
    timeCombo->insertItem("less than");
    timeCombo->insertItem("more than");
    timeCombo->insertItem("equal to");

    dateCombo = new QComboBox(dateEntry, 0);
    dateKey = new DateEdit(dateEntry, 0);

    /* Build the date combo list */
    dateCombo->insertItem("less than");
    dateCombo->insertItem("more than");
    dateCombo->insertItem("equal to");

    ts = 0;

    ws = new QWidgetStack(this, 0);
    ws->setMargin(0);
    ws->addWidget(textEntry, TVVariant::String);
    ws->addWidget(intEntry, TVVariant::Int);
    ws->addWidget(timeEntry, TVVariant::Time);
    ws->addWidget(dateEntry, TVVariant::Date);

    /* connect the signals down */
    connect(textKey, SIGNAL(textChanged(const QString&)), 
            this, SIGNAL(valueChanged()));
    connect(intKey, SIGNAL(valueChanged(int)), 
            this, SIGNAL(valueChanged()));
    connect(dateKey, SIGNAL(valueChanged(const QDate&)), 
            this, SIGNAL(valueChanged()));
    connect(timeKey, SIGNAL(valueChanged(const QTime&)), 
            this, SIGNAL(valueChanged()));

    connect(intCombo, SIGNAL(activated(int)), this, SIGNAL(valueChanged()));
    connect(textCombo, SIGNAL(activated(int)), this, SIGNAL(valueChanged()));
    connect(timeCombo, SIGNAL(activated(int)), this, SIGNAL(valueChanged()));
    connect(dateCombo, SIGNAL(activated(int)), this, SIGNAL(valueChanged()));

    ws->raiseWidget(TVVariant::String);
    layout->addWidget(ws);

    current_type = TVVariant::String;
}

/*!
    Destructs the widget 
*/
TVFilterKeyEntry::~TVFilterKeyEntry()
{
}

void TVFilterKeyEntry::setKey(int i) 
{

    if (!ts) return;
    if (!ts->kRep) return;

    /* set up to raise appropriate widget set */
    if (current_type != ts->kRep->getKeyType(i)) {
        current_type = ts->kRep->getKeyType(i);
        ws->raiseWidget(current_type);
    }
}

void TVFilterKeyEntry::setTableState(TableState *t) {
    int i;
    ts = t;
    if(!t) return;
    if (!t->kRep)
        return;
    if (t->kRep->getNumFields() < 1)
        return;
    setKey(0);
    /* set up the the menu stuff.. */
}

CmpType TVFilterKeyEntry::getCompareType()
{

    switch(current_type) {
        case TVVariant::String: {
            CmpType k = (CmpType) textCombo->currentItem();
            return k;
        }
        case TVVariant::Int: {
            CmpType k = (CmpType) intCombo->currentItem();
            return k;
        }
        case TVVariant::Time: {
            CmpType k = (CmpType) timeCombo->currentItem();
            return k;
        }
        case TVVariant::Date: {
            CmpType k = (CmpType) dateCombo->currentItem();
            return k;
        }
        default:
            break;
    }
    return ct_equal;
}

/* MUST return a valid pointer */
TVVariant TVFilterKeyEntry::getCompareValue()
{
    TVVariant sendkey;
    int tmp;

    switch(current_type) {
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
        default: {
            sendkey = TVVariant(0);
            qWarning("TVFilterKeyEntry::getCompareValue() "
                     "cannot work out data type");
        }
    }
    return sendkey;
}
