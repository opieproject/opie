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
#ifndef BrowseKeyEntry_H
#define BrowseKeyEntry_H

#include <qwidget.h>
#include "../db/common.h"

/* Forward class declarations */
class QWidgetStack;
class QToolButton;
class QPushButton;
class QLineEdit;
class DateEdit;
class TimeEdit;
class IntEdit;
class QPopupMenu;
class QHBox;

class TVBrowseKeyEntry: public QWidget
{
    Q_OBJECT
public:
    TVBrowseKeyEntry( QWidget *parent = 0, 
                        const char *name = 0, WFlags f = 0 );
    ~TVBrowseKeyEntry();

    void setTableState(TableState *t);
    void rebuildKeys();
    void reset();

signals:
    void searchOnKey(int keyIndex, TVVariant keyData);
    void sortChanged(int i);

private slots:
    void changeKeySlot(int);
    void changeKeyMenuSlot();
    void searchOnText();

private:
    QPushButton *resetButton;
    QToolButton *changeKeyButton;
    QPopupMenu *keyMenu;
    int totalKeys;
    TableState *ts;

    /* each type of possible data entry will be put on the stack */
    QWidgetStack *ws;

    /* include widgets for each type of data entry you need here. */
    QLineEdit *textKey;
    IntEdit *intKey;
    DateEdit *dateKey;
    TimeEdit *timeKey;
};

#endif
