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

class DateBookDayView : public QTable
{
    Q_OBJECT
public:
    DateBookDayView( bool hourClock, QWidget *parent, const char *name );
    bool whichClock() const;

public slots:
    void moveUp();
    void moveDown();

signals:
    void sigColWidthChanged();
    void sigCapturedKey( const QString &txt );
protected slots:
    void slotChangeClock( bool );
protected:
    virtual void paintCell( QPainter *p, int row, int col, const QRect &cr, bool selected );
    virtual void paintFocus( QPainter *p, const QRect &cr );
      
    virtual void resizeEvent( QResizeEvent *e );
    void keyPressEvent( QKeyEvent *e );
    void initHeader();
private:
    bool ampm;
};

class DateBookDay;
class DateBookDayWidget : public QWidget
{
    Q_OBJECT

public:
    DateBookDayWidget( const EffectiveEvent &e, DateBookDay *db );
    ~DateBookDayWidget();

    const QRect &geometry() { return geom; }
    void setGeometry( const QRect &r );

    const EffectiveEvent &event() const { return ev; }
        
signals:
    void deleteMe( const Event &e );
    void editMe( const Event &e );
    void beamMe( const Event &e );

protected:
    void paintEvent( QPaintEvent *e );
    void mousePressEvent( QMouseEvent *e );

private:
    const EffectiveEvent ev;
    DateBookDay *dateBook;
    QString text;
    QRect geom;
};

//Marker for current time in the dayview
class DateBookDayTimeMarker : public QWidget
{
    Q_OBJECT

public:
    DateBookDayTimeMarker( DateBookDay *db );
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
    DateBookDay *dateBook;
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
    void setSelectedWidget( DateBookDayWidget * );
    DateBookDayWidget * getSelectedWidget( void );

public slots:
    void setDate( int y, int m, int d );
    void setDate( QDate );
    void redraw();
    void slotWeekChanged( bool bStartOnMonday );
    void updateView();	//updates TimeMarker and DayWidget-colors

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
    DateBookDayWidget *intersects( const DateBookDayWidget *item, const QRect &geom );
    QDate currDate;
    DateBookDayView *view;
    DateBookDayHeader *header;
    DateBookDB *db;
    WidgetListClass widgetList;	//reimplemented QList for sorting widgets by height
    int startTime;
    DateBookDayWidget *selectedWidget; //actual selected widget (obviously)
    DateBookDayTimeMarker *timeMarker;	//marker for current time
};

#endif
