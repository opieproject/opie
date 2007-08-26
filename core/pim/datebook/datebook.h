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

#ifndef DATEBOOK_H
#define DATEBOOK_H

#include "datebooktypes.h"
#include "dateentryimpl.h"

#include <qpe/datebookdb.h>

#include <qmainwindow.h>

class QAction;
class QWidgetStack;
class DateBookDay;
class DateBookWeek;
class DateBookWeekLst;
class ODateBookMonth;
class Event;
class QDate;
class Ir;

class DateBook : public QMainWindow
{
    Q_OBJECT

public:
    static QString appName() { return QString::fromLatin1("datebook"); }
    DateBook( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    ~DateBook();

signals:
    void newEvent();
    void signalNotFound();
    void signalWrapAround();

protected:
    QDate currentDate();
    void timerEvent( QTimerEvent *e );
    void closeEvent( QCloseEvent *e );
    DateEntry *setupEditDlg( QDialog &editDlg, const Event &e, const QString &caption );
    void editEvent( const Event &e, const QDate *date );

    void view(int v, const QDate &d);

public slots:
    void flush();
    void reload();

private slots:
    void fileNew();
    void slotNewEntry(const QDateTime &start, const QDateTime &end, const QString &str, const QString &location=0);
    void slotSettings();
    void slotToday();   // view today
    void changeClock( bool newClock );
    void changeWeek( bool newDay );
    void appMessage(const QCString& msg, const QByteArray& data);
    // handle key events in the day view...
    void slotNewEventFromKey( const QString &str );
    void slotFind();
    void slotDoFind( const QString &, const QDate &, bool, bool, int );

    void viewDefault(const QDate &d);

    void viewDay();
    void viewWeek();
    void viewWeekLst();
    void viewMonth();

    void showDay( int y, int m, int d );

    void insertEvent( const Event &e );
    void editEvent( const EffectiveEvent &e );
    void duplicateEvent( const Event &e );
    void removeEvent( const EffectiveEvent &e );

    void receive( const QCString &msg, const QByteArray &data );
    void setDocument( const QString & );
    void beamEvent( const Event &e );
    void beamDone( Ir *ir );

private:
    void addEvent( const Event &e );
    void initDay();
    void initWeek();
    void initWeekLst();
    void initMonth();
    void loadSettings();
    void saveSettings();

private:
    DateBookDBHoliday *db;
    DateBookHoliday*db_holiday;
    QWidgetStack *views;
    DateBookDay *dayView;
    DateBookWeek *weekView;
    ODateBookMonth *monthView;
    DateBookWeekLst *weekLstView;
    QAction *dayAction, *weekAction, *weekLstAction, *monthAction;
    int weeklistviewconfig;
    bool aPreset;    // have everything set to alarm?
    int presetTime;  // the standard time for the alarm
    int startTime;
    int rowStyle;
    int defaultView;
    QArray<int> defaultCategories;
    QString defaultLocation;
    bool bJumpToCurTime; //should jump to current time in dayview?
    bool ampm;
    bool onMonday;

    bool syncing;
    bool inSearch;

    int alarmCounter;

    QString checkEvent(const Event &);
};

#endif
