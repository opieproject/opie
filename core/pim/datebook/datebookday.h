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
#ifndef DATEBOOKDAY_H
#define DATEBOOKDAY_H

#include <qpe/event.h>

#include <qdatetime.h>
#include <qtable.h>
#include <qvbox.h>
#include <qlist.h>

class DateBookDayHeader;
class DateBookDB;
class QDateTime;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class DateBookDay;
class DateBookDayView;
class DayViewLayout;

// Is both the 'all day' and '9-10' view.  depending on how its constructed.
class DateBookDayView : public QScrollView
{
    Q_OBJECT
public:
    enum Type {
	AllDay,
	ScrollingDay,
	CompressedDay
    };

    DateBookDayView( Type viewType, bool hourClock,
	    QWidget *parent = 0, const char *name = 0 );

    bool whichClock() const;

    void addOccurance(EffectiveEvent &);
    void takeOccurance(EffectiveEvent &);
    void clearItems();

    void layoutItems();

    int startHour() const { return QMIN(startSel, endSel); }
    int endHour() const { return QMAX(startSel, endSel) + 1; }

public slots:
    void startAtTime(int);
    void moveUp();  // scrolling with keys from DateBookDay
    void moveDown();

signals:
    void sigColWidthChanged();
    void sigCapturedKey( const QString &txt );

    void removeEvent( const Event& );
    void editEvent( const Event& );
    void beamEvent( const Event& );

protected slots:
    void slotChangeClock( bool );

protected:
    void resizeEvent( QResizeEvent *e );
    void keyPressEvent( QKeyEvent *e );
    void drawContents( QPainter * p, int x, int y, int w, int h );

    void contentsMousePressEvent( QMouseEvent * );
    void contentsMouseReleaseEvent( QMouseEvent * );
    void contentsMouseMoveEvent( QMouseEvent * );

private:

    bool ampm;
    Type typ;

    DayViewLayout *itemList;
    int hourAtPos(int) const;
    int posOfHour(int) const;
    //QList<DayItem> itemList;
    //DateBookDayWidget *intersects( const DateBookDayWidget *item, const QRect &geom );

    int startSel, endSel;
    bool dragging;

    int start_of_day;
};


class DateBookDay : public QVBox
{
    Q_OBJECT

public:
    DateBookDay( bool ampm, bool startOnMonday, DateBookDB *newDb,
		 QWidget *parent, const char *name );
    void selectedDates( QDateTime &start, QDateTime &end );
    QDate date() const;
    DateBookDayView *dayView() const { return view; }
    void setStartViewTime( int startHere );
    int startViewTime() const;

public slots:
    void setDate( int y, int m, int d );
    void setDate( QDate );
    void redraw();
    void slotWeekChanged( bool bStartOnMonday );

signals:
    void removeEvent( const Event& );
    void editEvent( const Event& );
    void beamEvent( const Event& );
    void newEvent();
    void sigNewEvent( const QString & );

protected slots:
    void keyPressEvent(QKeyEvent *);

private slots:
    void dateChanged( int y, int m, int d );
    void slotColWidthChanged() { relayoutPage(); };

private:
    void getEvents();
    void relayoutPage( bool fromResize = false );
    QDate currDate;
    DateBookDayView *view;
    DateBookDayView *allView;
    DateBookDayHeader *header;
    DateBookDB *db;
    int startTime;
};

#endif
