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
#ifndef FilterKeyEntry_H
#define FilterKeyEntry_H

#include <qwidget.h>
#include <qhbox.h>
#include <qlayout.h>
#include "../db/common.h"

/* Forward class declarations */
class QWidgetStack;
class QToolButton;
class QLineEdit;
class DateEdit;
class TimeEdit;
class IntEdit;
class QPopupMenu;
class QComboBox;

typedef enum _CmpType {
    ct_less = 0,
    ct_more,
    ct_equal,
    ct_contains,
    ct_startswith,
    ct_endswith
} CmpType;


class TVFilterKeyEntry: public QWidget
{
    Q_OBJECT
public:
    TVFilterKeyEntry( QWidget *parent = 0, 
                        const char *name = 0, WFlags f = 0 );
    ~TVFilterKeyEntry();

    void setTableState(TableState *t);
    void setKey(int i);

    CmpType getCompareType();
    TVVariant getCompareValue();

signals:
        void valueChanged();

private:
    /* include widgets for each type of data entry you need here. */
    QLineEdit *textKey;
    QComboBox *textCombo;
    QHBox *textEntry;

    IntEdit *intKey;
    QComboBox *intCombo;
    QHBox *intEntry;

    TimeEdit *timeKey;
    QComboBox *timeCombo;
    QHBox *timeEntry;

    DateEdit *dateKey;
    QComboBox *dateCombo;
    QHBox *dateEntry;

    TableState *ts;

    /* each type of possible data entry will be put on the stack */
    QWidgetStack *ws;

    /* This allows for the inherited functions dealing with prefered size
     * etc to simply get the information from the layout.  
     */
    QHBoxLayout *layout; 

    TVVariant::KeyType current_type;
};

#endif
