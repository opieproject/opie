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

#ifndef DATEBOOKDAY_H
#define DATEBOOKDAY_H

#include <opie2/opimevent.h>
#include <opie2/opimoccurrence.h>

#include <qdatetime.h>
#include <qtable.h>
#include <qvbox.h>
#include <qlist.h>
#include <qlineedit.h>

#include "dayview.h"

class DateBookDayHeader;
class DateBookDB;
class DateBookDBHoliday;
class DatebookdayAllday;
class DateBookDayView;
class QDateTime;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;

class DateBookDayViewQuickLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    DateBookDayViewQuickLineEdit(const QDateTime &start, const QDateTime &end, Opie::Datebook::DayView * dv, const char *name=0);
protected:
    Opie::OPimEvent quickEvent;
    Opie::Datebook::DayView *dayview;
    int active;
    void focusOutEvent( QFocusEvent *e );
    void mousePressEvent( QMouseEvent *e );
protected slots:
    void slotReturnPressed(void);
    void finallyCallClose();
signals:
    void insertEvent(const Opie::OPimEvent &);
};


class DateBookDayView : public QTable
{
    Q_OBJECT
public:
    DateBookDayView( bool hourClock, Opie::Datebook::DayView *dv, QWidget *parent, const char *name );
    bool whichClock() const;

    void setRowStyle( int style );
    void selectedDateTimes( QDateTime &start, QDateTime &end );

public slots:
    void moveUp();
    void moveDown();
    void slotDateChanged( int year, int month, int day );

signals:
    void sigColWidthChanged();
    void sigCapturedKey( const QString &txt );
    void sigDateChanged( int y, int m, int d );
protected slots:
    void slotChangeClock( bool );
protected:
    virtual void paintCell( QPainter *p, int row, int col, const QRect &cr, bool selected );
    virtual void paintFocus( QPainter *p, const QRect &cr );

    virtual void resizeEvent( QResizeEvent *e );
    void keyPressEvent( QKeyEvent *e );
    void contentsMouseReleaseEvent( QMouseEvent *e );
    void initHeader();
    void selectedHours( int &sh, int &eh );
private:
    bool ampm;
    QDate currDate;
    Opie::Datebook::DayView *dayview;
    DateBookDayViewQuickLineEdit *quickLineEdit;
};

class DateBookDayWidget : public QWidget
{
    Q_OBJECT

public:
    DateBookDayWidget( const Opie::OPimOccurrence &e, Opie::Datebook::DayView *dv );
    ~DateBookDayWidget();

    const QRect &geometry() { return geom; }
    void setGeometry( const QRect &r );

    const Opie::OPimOccurrence &event() const { return ev; }

protected:
    void paintEvent( QPaintEvent *e );
    void mousePressEvent( QMouseEvent *e );

private:
    /**
     * Sets the text for an all day Opie::OPimEvent
     * All day events have no time associated
     */
    void setAllDayText( QString& text );

    /**
     * Sets the EventText
     * it got a start and an end Time
     */
    void setEventText( QString& text );
    Opie::OPimOccurrence ev;
    Opie::Datebook::DayView *dayView;
    QString text;
    QRect geom;
};

//Marker for current time in the dayview
class DateBookDayTimeMarker : public QWidget
{
    Q_OBJECT

public:
    DateBookDayTimeMarker( DateBookDayView *dv );
    ~DateBookDayTimeMarker();

    const QRect &geometry() { return geom; }
    void setGeometry( const QRect &r );
    void setTime( const QTime &t );

signals:

protected:
    void paintEvent( QPaintEvent *e );

private:
    QRect geom;
    QTime time;
    DateBookDayView *dayView;
};

//reimplemented the compareItems function so that it sorts DayWidgets by geometry heights
class WidgetListClass : public QList<DateBookDayWidget>
{
    private:

    int compareItems( QCollection::Item s1, QCollection::Item s2 )
    {
        //hmm, don't punish me for that ;)
        if (reinterpret_cast<DateBookDayWidget*>(s1)->geometry().height() > reinterpret_cast<DateBookDayWidget*>(s2)->geometry().height())
        {
            return -1;
        } else
        {
            return 1;
        }
    }


};


#endif
