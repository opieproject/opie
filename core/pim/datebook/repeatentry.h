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

#ifndef REPEATENTRY_H
#define REPEATENTRY_H

#include "repeatentrybase.h"

#include <qpe/event.h>

#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qdatetime.h>
#include <qlist.h>
#include <qtoolbutton.h>

class DateBookMonth;

class RepeatEntry : public RepeatEntryBase
{
    Q_OBJECT
public:
    RepeatEntry( bool startOnMonday,
		 const QDate &start, QWidget *parent = 0, const char *name = 0,
                 bool modal = TRUE, WFlags fl = 0 );
    RepeatEntry( bool startOnMonday,
		 const Event::RepeatPattern &rp, const QDate &start,
		 QWidget *parent = 0, const char *name = 0, bool modal = TRUE,
		 WFlags fl = 0 );
    ~RepeatEntry();

    Event::RepeatPattern repeatPattern();
    QDate endDate() { return end; };

public slots:
    void slotSetRType( int );
    void endDateChanged( int, int, int );
    void slotNoEnd( bool unused );

private slots:
    void setupRepeatLabel( const QString& );
    void setupRepeatLabel( int );
    void slotWeekLabel();
    void slotMonthLabel( int );
    void slotChangeStartOfWeek( bool onMonday );

private:
    void setupNone();
    void setupDaily();
    void setupWeekly();
    void setupMonthly();
    void setupYearly();

    enum repeatButtons { NONE, DAY, WEEK, MONTH, YEAR };
    void init();
    inline void hideExtras();
    void showRepeatStuff();

    QList<QToolButton> listRTypeButtons;
    QList<QToolButton> listExtra;
    QDate start;    // only used in one spot...
    QDate end;
    repeatButtons currInterval;
    bool startWeekOnMonday;
    DateBookMonth *repeatPicker;
};

inline void RepeatEntry::hideExtras()
{
    // hide the extra buttons...
    fraExtra->hide();
    chkNoEnd->hide();
    QListIterator<QToolButton> it( listExtra );
    for ( ; *it; ++it ) {
	(*it)->hide();
	(*it)->setOn( FALSE );
    }

}

#endif
