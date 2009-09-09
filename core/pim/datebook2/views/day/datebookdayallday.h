/*
                             This file is part of the Opie Project

                             Copyright (C) Rajko Albrecht <>
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

#ifndef DATEBOOKDAYALLDAYBASE_H
#define DATEBOOKDAYALLDAYBASE_H

#include <qvariant.h>
#include <qframe.h>
#include <qscrollview.h>
#include <qlabel.h>
#include <qlist.h>

#include "opie2/opimevent.h"
#include "opie2/opimoccurrence.h"

#include "dayview.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class DatebookAlldayDisp;
class DatebookEventDesc;

class DatebookdayAllday : public QScrollView
{
    Q_OBJECT

public:
    DatebookdayAllday(Opie::Datebook::DayView *view,
            QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~DatebookdayAllday();
    DatebookAlldayDisp* addEvent(const Opie::OPimOccurrence &e);
    unsigned int items()const{return item_count;}
    void popup( const Opie::OPimOccurrence &e, const QPoint &pos );
    const Opie::OPimOccurrence *selectedEvent();

public slots:
    void removeAllEvents();

protected:
    QFrame * m_MainFrame;
    QVBoxLayout* datebookdayalldayLayout;
    DatebookEventDesc * lblDesc;
    unsigned int item_count;
    QList<DatebookAlldayDisp> subWidgets;
    Opie::Datebook::DayView *dayView;
    const Opie::OPimOccurrence *m_selectedEv;
};

class DatebookAlldayDisp : public QLabel
{
    Q_OBJECT

public:
    DatebookAlldayDisp(DatebookdayAllday *allday, const Opie::OPimOccurrence& e,
                       QWidget* parent=0,const char* name = 0, WFlags fl=0);
    virtual ~DatebookAlldayDisp();

public slots:

protected:
    Opie::OPimOccurrence m_Ev;
    void mousePressEvent( QMouseEvent *e );
    bool m_holiday:1;
    DatebookdayAllday *m_allday;
};

class DatebookEventDesc: public QLabel
{
    Q_OBJECT

public:
    DatebookEventDesc(QWidget* Parent=0,const char* name = 0);
    virtual ~DatebookEventDesc();

public slots:
    void disp_event(const Opie::OPimEvent &e);

protected:
    void mousePressEvent(QMouseEvent *e);
    QTimer* m_Timer;
};

#endif // DATEBOOKDAYALLDAYBASE_H
