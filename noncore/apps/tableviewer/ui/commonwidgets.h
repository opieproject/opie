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
#ifndef Dateedit_H
#define Dateedit_H

#include <qwidget.h>
#include <qdatetime.h>

/* inherited classes */
#include <qtoolbutton.h>
#include <qspinbox.h>

class DateBookMonth;
class QComboBox;

class DateEdit : public QToolButton
{
    Q_OBJECT

public:
    DateEdit( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    ~DateEdit();

    QDate date() const;
    void setDate(QDate);

    QSizePolicy sizePolicy() const;
signals:
    void valueChanged(const QDate &);

public slots:
    void clear();
private slots:
    void subValueChanged();

private:
    DateBookMonth *dateSelector;
};

class TimeEdit : public QWidget
{
    Q_OBJECT

public:
    TimeEdit( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    ~TimeEdit();

    QTime time() const;
    void setTime(QTime);

    QSizePolicy sizePolicy() const;
signals:
    void valueChanged(const QTime &);

public slots:
    void clear();
private slots:
    void subValueChanged();

private:
    QSpinBox *secondKey;
    QSpinBox *minuteKey;
    QSpinBox *hourKey;
    QComboBox *ampm;
};

/* more for consistency than need */
class IntEdit : public QSpinBox
{
    Q_OBJECT

public:
    IntEdit( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    ~IntEdit();

    int value();

public slots:
    void clear();
};
#endif
