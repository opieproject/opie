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
#ifndef DATEBOOK_H
#define DATEBOOK_H

#include <qpe/datebookdb.h>

#include <qmainwindow.h>

class QAction;
class QWidgetStack;
class DateBookDay;
class DateBookWeek;
class DateBookWeekLst;
class DateBookMonth;
class Event;
class QDate;
class Ir;

class DateBookDBHack : public DateBookDB {
 public:
  Event eventByUID(int id);
};

class DateBook : public QMainWindow
{
    Q_OBJECT

public:
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

    void view(int v, const QDate &d);

public slots:
    void flush();
    void reload();

private slots:
    void fileNew();
    void slotNewEntry(const QDateTime &start, const QDateTime &end, const QString &str);
    void slotSettings();
    void newDefaultView(QAction *a);
    void slotToday();	// view today
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

    void editEvent( const Event &e );
    void removeEvent( const Event &e );

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
    DateBookDBHack *db;
    QWidgetStack *views;
    DateBookDay *dayView;
    DateBookWeek *weekView;
    DateBookMonth *monthView;
    DateBookWeekLst *weekLstView;
    QAction *dayAction, *weekAction, *weekLstAction, *monthAction;
    bool aPreset;    // have everything set to alarm?
    int presetTime;  // the standard time for the alarm
    int startTime;
    bool ampm;
    bool onMonday;

    bool syncing;
    bool inSearch;

    QString checkEvent(const Event &);
};

#endif
