#include "weeklstview.h"
#include "datebookweeklstview.h"
#include "datebooktypes.h"
#include "datebookweeklstdayhdr.h"
#include "datebookweeklstheader.h"
#include "datebookweeklstevent.h"

#include <opie2/odebug.h>

#include <qpe/config.h>
#include <qpe/qpeapplication.h>

#include <qlayout.h>
#include <qtl.h>

using namespace Opie;
using namespace Opie::Datebook;

DateBookWeekLstView::DateBookWeekLstView(OPimOccurrence::List &ev,
                     WeekLstView *view,
                     const QDate &d, bool onM, int timeDisplay, bool showAmPm,
                     QWidget* parent,
                     const char* name, WFlags fl)
    : QWidget( parent, name, fl ), ampm(showAmPm)
{
    childs.clear();
    weekLstView = view;
    m_MainLayout = new QVBoxLayout( this );
    setEvents(ev,d,onM,timeDisplay);
}

void DateBookWeekLstView::setEvents(OPimOccurrence::List &ev, const QDate &d, bool onM, int timeDisplay)
{
    QValueList<QObject*>::Iterator wIter;
    for (wIter=childs.begin();wIter!=childs.end();++wIter) {
        QObject*w = (*wIter);
        delete w;
    }
    childs.clear();

    setUpdatesEnabled(false);
//    m_MainLayout->deleteAllItems();

    bStartOnMonday=onM;
    setPalette(white);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

    qBubbleSort(ev);
    OPimOccurrence::List::ConstIterator it;
    it=ev.begin();

    int dayOrder[7];
    if (bStartOnMonday) {
        for (int dayIdx=0; dayIdx<7; dayIdx++)
            dayOrder[dayIdx]=dayIdx+1;
    } else {
        for (int dayIdx=0; dayIdx<7; dayIdx++)
            dayOrder[dayIdx]=dayIdx;

        dayOrder[0]=7;
    }

    // Calculate offset to first day of week.
    int dayoffset=d.dayOfWeek();
    if(bStartOnMonday) dayoffset--;
    else if( dayoffset == 7 ) dayoffset = 0;

    for (int i=0; i<7; i++) {
        QWidget*w = new QWidget(this);
        w->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        w->setPalette(white);
        QVBoxLayout * tlayout = new QVBoxLayout(w);
        childs.append(w);
        // Header
        DateBookWeekLstDayHdr *hdr=new DateBookWeekLstDayHdr(d.addDays(i-dayoffset), bStartOnMonday,w);
        connect(hdr, SIGNAL(showDate(int,int,int)), this, SIGNAL(showDate(int,int,int)));
        connect(hdr, SIGNAL(addEvent(const QDateTime&,const QDateTime&)),
            this, SIGNAL(addEvent(const QDateTime&,const QDateTime&)));
        tlayout->addWidget(hdr);
        connect( qApp, SIGNAL(clockChanged(bool)), this, SLOT(slotClockChanged(bool)));

        // Events
        while ( (*it).date().dayOfWeek() == dayOrder[i] && it!=ev.end() ) {
//X            if(!(((*it).endTime().hour()==0) && ((*it).endTime().minute()==0) && ((*it).startDate()!=(*it).date()))) {  // Skip events ending at 00:00 starting at another day.
                DateBookWeekLstEvent *l=new DateBookWeekLstEvent( ampm, *it, weekLstView, timeDisplay, w );
                tlayout->addWidget(l);
                connect (l, SIGNAL(redraw()), this, SIGNAL(redraw()));
//X            }
            it++;
        }
        tlayout->addItem(new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::Expanding));
        m_MainLayout->addWidget(w);
    }
    setUpdatesEnabled(true);
}

void DateBookWeekLstView::slotClockChanged( bool ap ) {
    ampm = ap;
}

DateBookWeekLstView::~DateBookWeekLstView()
{}

void DateBookWeekLstView::keyPressEvent(QKeyEvent *e)
{
    e->ignore();
}
