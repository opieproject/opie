/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
** Copyright (C) 2005 Bernhard Rosenkraenzer <bero@arklinux.org>. All
** rights reserved.
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
#ifndef DATEBOOKMONTH
#define DATEBOOKMONTH

#include <qtopia/private/event.h>

#include <QDateTime>
#include <QList>
#include <QTableWidget>
#include <QPushButton>
#include <QKeyEvent>
#include <QMenu>
#include <QRect>
#include <QPainter>
#include <QColorGroup>

#include "calendar.h"
#include "timestring.h"

class QToolButton;
class QComboBox;
class QSpinBox;
class Event;
class DateBookDB;

class DateBookMonthHeaderPrivate;
class DateBookMonthHeader : public QWidget
{
    Q_OBJECT

public:
    DateBookMonthHeader( QWidget *parent = 0 );
    ~DateBookMonthHeader();
    void setDate( int year, int month );

signals:
    void dateChanged( int year, int month );

protected slots:
    void keyPressEvent(QKeyEvent *e ) {
	e->ignore();
    }

private slots:
    void updateDate();
    void firstMonth();
    void lastMonth();
    void monthBack();
    void monthForward();

private:
    QToolButton *begin, *back, *next, *end;
    QComboBox *month;
    QSpinBox *year;
    DateBookMonthHeaderPrivate *d;
    int focus;
};

class DayItemMonthPrivate;
class DayItemMonth : public QTableWidgetItem
{
public:
    DayItemMonth( const QString &t, int type=Type );
    ~DayItemMonth();
    void paint( QPainter *p, const QPalette &pal, const QRect &cr, bool selected );
    void setDay( int d ) { dy = d; }
    void setEvents( const QList<Event> &events ) { daysEvents = events; };
    void setEvents( const QList<EffectiveEvent> &effEvents );
    void clearEvents() { daysEvents.clear(); };
    void clearEffEvents();
    int day() const { return dy; }
    void setType( Calendar::Day::Type t );
    Calendar::Day::Type type() const { return typ; }

private:
    QBrush back;
    QColor forg;
    int dy;
    Calendar::Day::Type typ;
    QList<Event> daysEvents; // not used anymore...
    DayItemMonthPrivate *d;
};

class DateBookMonthTablePrivate;
class DateBookMonthTable : public QTableWidget
{
    Q_OBJECT

public:
    DateBookMonthTable( QWidget *parent = 0, DateBookDB *newDb = 0 );
    ~DateBookMonthTable();
    void setDate( int y, int m, int d );
    void redraw();

    QSize minimumSizeHint() const { return sizeHint(); }
    QSize minimumSize() const { return sizeHint(); }
    void  getDate( int& y, int &m, int &d ) const {y=selYear;m=selMonth;d=selDay;}
    void setWeekStart( bool onMonday );
signals:
    void dateClicked( int year, int month, int day );

protected:
    void viewportMouseReleaseEvent( QMouseEvent * );

protected slots:

    void keyPressEvent(QKeyEvent *e ) {
	e->ignore();
    }

private slots:
    void dayClicked( int row, int col );
    void dragDay( int row, int col );

private:
    void setupTable();
    void setupLabels();

    void findDay( int day, int &row, int &col );
    void getEvents();
    void changeDaySelection( int row, int col );

    int year, month, day;
    int selYear, selMonth, selDay;
    QList<Event> monthsEvents; // not used anymore...
    DateBookDB *db;
    DateBookMonthTablePrivate *d;
};

class DateBookMonthPrivate;
class DateBookMonth : public QWidget
{
    Q_OBJECT

public:
    /* ac = Auto Close */
    DateBookMonth( QWidget *parent = 0, bool ac = FALSE,
                   DateBookDB *data = 0 );
    ~DateBookMonth();
    QDate  selectedDate() const;

signals:
    /* ### FIXME add a signal with QDate -zecke */
    void dateClicked( int year, int month, int day );

public slots:
    void setDate( int y, int m );
    void setDate( int y, int m, int d );
    void setDate( QDate );
    void redraw();
    void slotWeekChange( bool );

protected slots:
    virtual void keyPressEvent(QKeyEvent *e);

private slots:
    void forwardDateClicked( int y, int m, int d ) { emit dateClicked(  y, m, d  ); }
    void finalDate(int, int, int);

private:
    DateBookMonthHeader *header;
    DateBookMonthTable *table;
    int year, month, day;
    bool autoClose;
    class DateBookMonthPrivate *d;
};

class DateButton : public QPushButton
{
    Q_OBJECT

public:
    DateButton( bool longDate, QWidget *parent );
    QDate date() const { return currDate; }

    bool customWhatsThis() const;

signals:
    void dateSelected( int year, int month, int day );

public slots:
    void setDate( int y, int m, int d );
    void setDate( QDate );
    void setWeekStartsMonday( int );
    void setDateFormat( DateFormat );

private slots:
    void pickDate();
    void gotHide();

private:
    bool longFormat;
    bool weekStartsMonday;
    QDate currDate;
    DateFormat df;
};

// this class is only here for Sharp ROM compatibility
// I have reverse engineered this class and it seems to
// work (only qtmail seems to use it)   - sandman
// DO NOT USE IT IN NEW CODE !!

class DateBookMonthPopup : public QMenu 
{
	Q_OBJECT
public:
	DateBookMonthPopup ( QWidget *w );

private:
	DateBookMonth *m_dbm;
};

#endif
