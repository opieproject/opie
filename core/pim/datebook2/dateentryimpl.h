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

#ifndef DATEENTRY_H
#define DATEENTRY_H

#include "dateentry.h"
#include "noteentryimpl.h"
#include "editor.h"

#include <opie2/opimevent.h>
#include <opie2/opimrecurrence.h>

#include <qdatetime.h>

class DateBookMonth;

class DateEntry : public DateEntryBase
{
    Q_OBJECT

public:
    DateEntry( bool startOnMonday, bool whichCLock = FALSE,
               QWidget* parent = 0, const char* name = 0 );
    ~DateEntry();

    void setEvent( const Opie::OPimEvent & );
    Opie::OPimEvent event();
    void setAlarmEnabled( bool alarmPreset, int presetTime/*X, Opie::OPimEvent::SoundTypeChoice*/ );
    virtual bool eventFilter( QObject *, QEvent * );
public slots:
    void endDateChanged( int, int, int );
    void endTimeChanged( const QString & );
    void endTimeChanged( const QTime & );
    void startDateChanged(int, int, int);
    void startTimeEdited( const QString & );
    void startTimeChanged( const QTime & );
    void startTimePicked( const QTime & );
    void typeChanged( const QString & );
    void slotRepeat();
    void slotChangeClock( bool );
    void slotChangeStartOfWeek( bool );
    void slotEditNote();

private:
    void init();
    void initCombos();
    void setDates( const QDateTime& s, const QDateTime& e );
    void setRepeatLabel();
    void updateTimeEdit(bool,bool);

    DateBookMonth *startPicker, *endPicker;
    QDate startDate, endDate;
    QTime startTime, endTime;
    Opie::OPimRecurrence rp;
    bool ampm:1;
    bool startWeekOnMonday:1;
    bool m_showStart:1;
    QString exceptions;

    QString noteStr;
};

namespace Opie {
namespace Datebook {
class DateEntryEditor : public Editor
{
public:
    DateEntryEditor( MainWindow*, QWidget* parent );

    virtual bool newEvent( const QDate& );
    virtual bool newEvent( const QDateTime& start, const QDateTime& end );
    virtual bool edit( const OPimEvent&, bool showRec = TRUE );

    virtual OPimEvent event() const;

protected:
    bool showDialog( QString caption, OPimEvent& event );
    QString checkEvent(const OPimEvent &e);

    OPimEvent m_event;
    QWidget *m_parent;
};
}
}

#endif // DATEENTRY_H
