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

#include "datebookdayallday.h"

#include <qlayout.h>
#include <qpe/ir.h>
#include <qpopupmenu.h>
#include <qtimer.h>

#include "datebookday.h"

using namespace Opie;
using namespace Opie::Datebook;

/*
 *  Constructs a DatebookdayAllday which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
DatebookdayAllday::DatebookdayAllday(DateBookDB* db, QWidget* parent,  const char* name, WFlags  )
    : QScrollView( parent, name ),item_count(0),dateBook(db)
{
    if ( !name )
        setName( "DatebookdayAllday" );
    setMinimumSize( QSize( 0, 0 ) );
    setMaximumHeight(3* (QFontMetrics(font()).height()+4) );

    m_MainFrame = new QFrame(viewport());
    m_MainFrame->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
    setFrameStyle(QFrame::NoFrame|QFrame::Plain);
    //setResizePolicy( QScrollView::Default );
    setResizePolicy(QScrollView::AutoOneFit);
    setHScrollBarMode( AlwaysOff );
    addChild(m_MainFrame);

    datebookdayalldayLayout = new QVBoxLayout( m_MainFrame );
    datebookdayalldayLayout->setSpacing( 0 );
    datebookdayalldayLayout->setMargin( 0 );

    lblDesc = new DatebookEventDesc(parent->parentWidget(),"");
    lblDesc->setBackgroundColor(Qt::yellow);
    lblDesc->hide();
    subWidgets.setAutoDelete(true);
}

/*
 *  Destroys the object and frees any allocated resources
 */
DatebookdayAllday::~DatebookdayAllday()
{
    // no need to delete child widgets, Qt does it all for us
}

DatebookAlldayDisp* DatebookdayAllday::addEvent(const OPimOccurrence&ev)
{
    DatebookAlldayDisp * lb;
    lb = new DatebookAlldayDisp(dateBook,ev,m_MainFrame,NULL);
    lb->show();
    datebookdayalldayLayout->addWidget(lb);
    subWidgets.append(lb);

    connect(lb,SIGNAL(displayMe(const OPimEvent&)),lblDesc,SLOT(disp_event(const OPimEvent&)));
    ++item_count;

    return lb;
}

DatebookAlldayDisp* DatebookdayAllday::addHoliday(const QString&e)
{
    DatebookAlldayDisp * lb;
    lb = new DatebookAlldayDisp(e,m_MainFrame,NULL);
    lb->show();
    datebookdayalldayLayout->addWidget(lb);
    subWidgets.append(lb);

    connect(lb,SIGNAL(displayMe(const OPimEvent&)),lblDesc,SLOT(disp_event(const OPimEvent&)));
    ++item_count;

    return lb;
}

void DatebookdayAllday::removeAllEvents()
{
    subWidgets.clear();
    item_count = 0;
}

DatebookAlldayDisp::DatebookAlldayDisp(DateBookDB *db,const OPimOccurrence& ev,
                                       QWidget* parent,const char* name,WFlags f)
    : QLabel(parent,name,f),m_Ev(ev),dateBook(db)
{
    QString strDesc = m_Ev.summary();
    strDesc = strDesc.replace(QRegExp("<"),"&#60;");
    setBackgroundColor(yellow);
    setText(strDesc);
    setFrameStyle(QFrame::Raised|QFrame::Panel);

    int s = QFontMetrics(font()).height()+4;
    setMaximumHeight( s );
    setMinimumSize( QSize( 0, s ) );
    m_holiday = false;
}

DatebookAlldayDisp::DatebookAlldayDisp(const QString&aholiday,QWidget* parent,const char* name, WFlags fl)
    : QLabel(parent,name,fl),m_Ev(),dateBook(0)
{
    QString strDesc = aholiday;
    strDesc = strDesc.replace(QRegExp("<"),"&#60;");
    OPimEvent ev;
    ev.setDescription(strDesc);
    ev.setAllDay(true);
//X    m_Ev.setEvent(ev);
    setText(strDesc);

    setAlignment(AlignHCenter);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum));

    //setFrameStyle(QFrame::Raised|QFrame::Panel);
    //setBackgroundColor(yellow);

    int s = QFontMetrics(font()).height()+4;
    setMaximumHeight( s );
    setMinimumSize( QSize( 0, s ) );

    m_holiday = true;
}

DatebookAlldayDisp::~DatebookAlldayDisp()
{
}

void DatebookAlldayDisp::beam_single_event()
{
    if (m_holiday) return;
    // create an OPimEvent and beam it...
    /*
     * Start with the easy stuff. If start and  end date is the same we can just use
     * the values of effective m_Events
     * If it is a multi day m_Event we need to find the real start and end date...
     */
    // FIXME I'm sure we have code duplication here...
    if ( m_Ev.toEvent().startDateTime().date() == m_Ev.toEvent().endDateTime().date() ) {
        OPimEvent m_Event( m_Ev.toEvent() );

        QDateTime dt( m_Ev.date(), m_Ev.startTime() );
        m_Event.setStartDateTime( dt );

        dt.setTime( m_Ev.endTime() );
        m_Event.setEndDateTime( dt );
        emit beamMe( m_Event );
    }else {
        /*
         * at least the the Times are right now
         */
        QDateTime start( m_Ev.toEvent().startDateTime() );
        QDateTime end  ( m_Ev.toEvent().endDateTime() );

        /*
         * ok we know the start date or we need to find it
         */
        if ( m_Ev.startTime() != QTime( 0, 0, 0 ) ) {
            start.setDate( m_Ev.date() );
        }else {
//X            QDate dt = DateBookDay::findRealStart( m_Ev.event().uid(), m_Ev.date(), dateBook );
//X            start.setDate( dt );
        }

        /*
         * ok we know now the end date...
         * else
         *   get to know the offset btw the real start and real end
         *   and then add it to the new start date...
         */
        if ( m_Ev.endTime() != QTime(23, 59, 59 ) ) {
            end.setDate( m_Ev.date() );
        }else{
            int days = m_Ev.toEvent().startDateTime().date().daysTo( m_Ev.toEvent().endDateTime().date() );
            end.setDate( start.date().addDays( days ) );
        }
        OPimEvent m_Event( m_Ev.toEvent() );
        m_Event.setStartDateTime( start );
        m_Event.setEndDateTime( end );
        emit beamMe( m_Event );
    }
}

void DatebookAlldayDisp::mousePressEvent(QMouseEvent*e)
{
    OPimEvent ev = m_Ev.toEvent();
    QColor b = backgroundColor();
    setBackgroundColor(green);
    update();
    QPopupMenu m;
    if (ev.isValidUid()) {
        m.insertItem( DateBookDayWidget::tr( "Edit" ), 1 );
        m.insertItem( DateBookDayWidget::tr( "Duplicate" ), 4 );
        m.insertItem( DateBookDayWidget::tr( "Delete" ), 2 );
        if(Ir::supported()) {
            m.insertItem( DateBookDayWidget::tr( "Beam" ), 3 );
            if(m_Ev.toEvent().hasRecurrence() ) 
                m.insertItem( DateBookDayWidget::tr( "Beam this occurence"), 5 );
        }
    }
    m.insertItem( tr( "Info"),6);
    int r = m.exec( e->globalPos() );
    setBackgroundColor(b);
    update();
    switch (r) {
    case 1:
        emit editMe( ev );
        break;
    case 2:
        emit deleteMe( ev );
        break;
    case 3:
        emit beamMe( ev );
        break;
    case 4:
        emit duplicateMe( ev );
        break;
    case 5:
        beam_single_event();
        break;
    case 6:
        emit displayMe( ev );
        break;
    default:
        break;
    }
}

DatebookEventDesc::DatebookEventDesc(QWidget*parent,const char*name)
    :QLabel(parent,name)
{
   m_Timer=new QTimer(this);
   connect(m_Timer,SIGNAL(timeout()),this,SLOT(hide()));
   setFrameStyle(QFrame::Sunken|QFrame::Panel);
   setTextFormat(RichText);
}

DatebookEventDesc::~DatebookEventDesc()
{
}

void DatebookEventDesc::mousePressEvent(QMouseEvent*)
{
    hide();
    if (m_Timer->isActive()) m_Timer->stop();
}

void DatebookEventDesc::disp_event(const OPimEvent&e)
{
    if (m_Timer->isActive()) m_Timer->stop();
    QString text;
    text = "<b><i>"+e.description()+"</i></b><br>";
    if (e.note().length()>0) {
        text+="<b>"+e.note()+"</b><br>";
    }
    if (e.location().length()>0) {
        text+="<i>"+e.location()+"</i><br>";
    }
    text = text.replace(QRegExp("\n"),"<br>");
    setText(text);
    QSize s = sizeHint();
    s+=QSize(10,10);
    resize(s);
    move( QMAX(0,(parentWidget()->width()-width()) / 2),
            (parentWidget()->height()-height())/2 );
    show();
    m_Timer->start(2000,true);
}
