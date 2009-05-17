/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
:     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef REPEATENTRY_H
#define REPEATENTRY_H

#include "repeatentrybase.h"

#include <opie2/opimrecurrence.h>

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
        const Opie::OPimRecurrence &rp, const QDate &start,
        QWidget *parent = 0, const char *name = 0, bool modal = TRUE,
        WFlags fl = 0 );
    ~RepeatEntry();

    Opie::OPimRecurrence recurrence();
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
    void slotEditExceptions();

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
    void updateEndDate();

    QList<QToolButton> listRTypeButtons;
    QList<QToolButton> listExtra;
    QDate start;    // only used in one spot...
    QDate end;
    repeatButtons currInterval;
    bool startWeekOnMonday;
    DateBookMonth *repeatPicker;
    Opie::OPimRecurrence::ExceptionList exceptions;
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
