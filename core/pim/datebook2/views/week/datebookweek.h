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

#ifndef DATEBOOKWEEK
#define DATEBOOKWEEK

#include <opie2/opimevent.h>
#include <opie2/opimoccurrence.h>

#include <qlist.h>
#include <qscrollview.h>
#include <qstring.h>
#include <qvaluelist.h>

class DateBookWeekHeader;
class QDate;
class QLabel;
class QResizeEvent;
class QSpinBox;
class QTimer;
class QHeader;

class DateBookWeekItem
{
public:
    DateBookWeekItem( const Opie::OPimOccurrence e );

    void setGeometry( int x, int y, int w, int h );
    QRect geometry() const { return r; }

    const QColor &color() const { return c; }
    const Opie::OPimOccurrence event() const { return ev; }

private:
    const Opie::OPimOccurrence ev;
    QRect r;
    QColor c;
};

class DateBookWeekView : public QScrollView
{
    Q_OBJECT
public:
    DateBookWeekView( bool ampm, bool weekOnMonday, QWidget *parent = 0,
              const char *name = 0 );

    bool whichClock() const;
    void showEvents( QValueList<Opie::OPimOccurrence> &ev );
    void moveToHour( int h );
    void setStartOfWeek( bool bOnMonday );

signals:
    void showDay( int d );
    void signalShowEvent( const Opie::OPimOccurrence & );
    void signalHideEvent();
    void signalDateChanged( QDate &date );

public slots:
    void slotDateChanged( QDate &date );

private slots:
    void slotChangeClock( bool );
    void alterDay( int );

protected:
    void keyPressEvent(QKeyEvent *);

private:
    void positionItem( DateBookWeekItem *i );
    DateBookWeekItem *intersects( const DateBookWeekItem * );
    void drawContents( QPainter *p, int cx, int cy, int cw, int ch );
    void contentsMousePressEvent( QMouseEvent * );
    void contentsMouseReleaseEvent( QMouseEvent * );
    void resizeEvent( QResizeEvent * );
    void initNames();

private:
    bool ampm;
    bool bOnMonday;
    QHeader *header;
    QList<DateBookWeekItem> items;
    int rowHeight;
    bool showingEvent;
    QDate m_date;
};

#endif
