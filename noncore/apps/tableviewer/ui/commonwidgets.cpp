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

#include <qlineedit.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <datebookmonth.h>
#include <qpopupmenu.h>
#include <qspinbox.h>
#include "commonwidgets.h"

DateEdit::DateEdit( QWidget *parent = 0, const char *name = 0, WFlags f = 0 )
    : QToolButton(parent, name)
{
    QPopupMenu *m1 = new QPopupMenu(this);
    dateSelector = new DateBookMonth(m1, 0, TRUE);
    m1->insertItem(dateSelector);
    setPopup(m1);
    setPopupDelay(0);

    connect(dateSelector, SIGNAL(dateClicked(int, int, int)),
        this, SLOT(subValueChanged()));
        
    setText(dateSelector->selectedDate().toString());
}


DateEdit::~DateEdit() {}

QDate DateEdit::date() const
{
    return dateSelector->selectedDate();
}

void DateEdit::setDate(QDate d) 
{
    dateSelector->setDate(d.year(), d.month(), d.day());
    setText(d.toString());
}

QSizePolicy DateEdit::sizePolicy() const
{
    QSizePolicy sp;
    sp.setHorData(QToolButton::sizePolicy().horData());
    sp.setVerData(QSizePolicy::Fixed);

    return sp;
}

void DateEdit::clear() 
{
    QDate today = QDate::currentDate();

    dateSelector->setDate(today.year(), today.month(), today.day());
    setText(today.toString());
}

void DateEdit::subValueChanged()
{
    QDate current = dateSelector->selectedDate();

    setText(current.toString());
    emit valueChanged(current);
}

TimeEdit::TimeEdit( QWidget *parent = 0, const char *name = 0, WFlags f = 0 )
    : QWidget(parent, name, f)
{
    QHBoxLayout *layout = new QHBoxLayout(this, 0);

    layout->addWidget(hourKey = new QSpinBox(1, 12, 1, this));
    hourKey->setWrapping(true);
    hourKey->setMinimumWidth(30);
    hourKey->setMaximumWidth(35);

    layout->addWidget(new QLabel(" : ", this));
    layout->addWidget(minuteKey = new QSpinBox(0, 59, 1, this));
    minuteKey->setWrapping(true);
    minuteKey->setMinimumWidth(30);
    minuteKey->setMaximumWidth(35);

    layout->addWidget(new QLabel(" : ", this));
    layout->addWidget(secondKey = new QSpinBox(0, 59, 1, this, 0));
    secondKey->setWrapping(true);
    secondKey->setMinimumWidth(30);
    secondKey->setMaximumWidth(35);

    layout->addWidget(ampm = new QComboBox(this));
    ampm->insertItem("AM");
    ampm->insertItem("PM");

    layout->addStretch(-1);

    clear();

    connect(secondKey, SIGNAL(valueChanged(const QString&)),
        this, SLOT(subValueChanged()));
    connect(minuteKey, SIGNAL(valueChanged(const QString&)),
        this, SLOT(subValueChanged()));
    connect(hourKey, SIGNAL(valueChanged(const QString&)),
        this, SLOT(subValueChanged()));
    connect(ampm, SIGNAL(activated(int)),
        this, SLOT(subValueChanged()));
}


TimeEdit::~TimeEdit() {}

QTime TimeEdit::time() const
{
    int s,m,h;

    s = secondKey->text().toInt();
    m = minuteKey->text().toInt();
    h = hourKey->text().toInt();

    if(ampm->currentItem() == 1) {
        /* pm */
        h = h + 12;
    }
    /* hour now ranges 1->24 */

    if (h == 12) 
        h = 0;
    if (h == 24) 
        h = 12;

    if(QTime::isValid(h, m, s))
        return QTime(h, m, s);
    return QTime(0, 0, 0);
}

void TimeEdit::setTime(QTime t) 
{
    int h = t.hour();
    secondKey->setValue(t.second());
    minuteKey->setValue(t.minute());

    /* h 0..23 */
    if (h > 11) {
        h -= 12;
        ampm->setCurrentItem(1);
    } else {
        ampm->setCurrentItem(0);
    }

    if (h == 0) h = 12;
    hourKey->setValue(h);
}

QSizePolicy TimeEdit::sizePolicy() const
{
    QSizePolicy sp;
    sp.setHorData(QSizePolicy::Preferred);
    sp.setVerData(QSizePolicy::Fixed);

    return sp;
}

void TimeEdit::clear() 
{
    secondKey->setValue(0);
    minuteKey->setValue(0);
    hourKey->setValue(12);

    ampm->setCurrentItem(0);
}

void TimeEdit::subValueChanged()
{
    emit valueChanged(time());
}

IntEdit::IntEdit( QWidget *parent = 0, const char *name = 0, WFlags f = 0 )
    : QSpinBox(INT_MIN, INT_MAX, 1, parent, name)
{
    setValue(0);
}


IntEdit::~IntEdit() {}

int IntEdit::value() 
{
    return cleanText().toInt();
}

void IntEdit::clear() 
{
    setValue(0);
}
