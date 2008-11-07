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
#include "view.h"

using namespace Opie;
using namespace Opie::Datebook;

//---------------------------------------------------------------------------

/*
 *  Constructs a DatebookdayAllday which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
DatebookdayAllday::DatebookdayAllday(DayView *view, QWidget* parent,  const char* name, WFlags  )
    : QScrollView( parent, name ),item_count(0)
{
    if ( !name )
        setName( "DatebookdayAllday" );

    dayView = view;
    m_selectedEv = NULL;

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
    lb = new DatebookAlldayDisp(this, ev, m_MainFrame, NULL);
    lb->show();
    datebookdayalldayLayout->addWidget(lb);
    subWidgets.append(lb);

//X    connect(lb,SIGNAL(displayMe(const OPimEvent&)),lblDesc,SLOT(disp_event(const OPimEvent&)));
    ++item_count;

    return lb;
}

void DatebookdayAllday::removeAllEvents()
{
    subWidgets.clear();
    item_count = 0;
}

const OPimOccurrence *DatebookdayAllday::selectedEvent()
{
    return m_selectedEv;
}

void DatebookdayAllday::popup( const Opie::OPimOccurrence &e, const QPoint &pos )
{
    m_selectedEv = &e;
    dayView->setSelectedWidget( NULL );
    dayView->popup( pos );
}

//---------------------------------------------------------------------------

DatebookAlldayDisp::DatebookAlldayDisp(DatebookdayAllday *allday, const OPimOccurrence& ev,
                                       QWidget* parent,const char* name,WFlags f)
    : QLabel(parent,name,f),m_Ev(ev)
{
    m_allday = allday;
    m_holiday = m_Ev.toEvent().isValidUid();

    QString strDesc = View::occurrenceDesc( m_Ev );
    strDesc = strDesc.replace(QRegExp("<"),"&#60;");
    setText(strDesc);

    int s = QFontMetrics(font()).height()+4;
    setMaximumHeight( s );
    setMinimumSize( QSize( 0, s ) );

    if(m_holiday) {
        setAlignment(AlignHCenter);
        setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum));
    }
    else {
        setBackgroundColor(yellow);
        setFrameStyle(QFrame::Raised|QFrame::Panel);
    }
}

DatebookAlldayDisp::~DatebookAlldayDisp()
{
}

void DatebookAlldayDisp::mousePressEvent(QMouseEvent*e)
{
    m_allday->popup( m_Ev, e->globalPos() );
}

//---------------------------------------------------------------------------

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
    if (m_Timer->isActive()) 
        m_Timer->stop();
}

void DatebookEventDesc::disp_event(const OPimEvent&e)
{
    if (m_Timer->isActive()) 
        m_Timer->stop();

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
