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

DateBookWeekLstView::DateBookWeekLstView(QValueList<EffectiveEvent> &ev,
                     const QDate &d, bool onM, bool showAmPm,
                     QWidget* parent,
                     const char* name, WFlags fl)
    : QWidget( parent, name, fl ), ampm(showAmPm)
{
    childs.clear();
    m_MainLayout = new QVBoxLayout( this );
    setEvents(ev,d,onM);
}

void DateBookWeekLstView::setEvents(QValueList<EffectiveEvent> &ev, const QDate &d, bool onM)
{
    QValueList<QObject*>::Iterator wIter;
    for (wIter=childs.begin();wIter!=childs.end();++wIter) {
        QObject*w = (*wIter);
        delete w;
    }
    childs.clear();

    setUpdatesEnabled(false);
//    m_MainLayout->deleteAllItems();
    Config config("DateBook");
    config.setGroup("Main");
    int weeklistviewconfig=config.readNumEntry("weeklistviewconfig", NORMAL);
    odebug << "weeklistviewconfig: " << weeklistviewconfig << oendl;

    bStartOnMonday=onM;
    setPalette(white);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

    qBubbleSort(ev);
    QValueListIterator<EffectiveEvent> it;
    it=ev.begin();

    int dayOrder[7];
    if (bStartOnMonday) {
        for (int d=0; d<7; d++) dayOrder[d]=d+1;
    } else {
        for (int d=0; d<7; d++) dayOrder[d]=d;
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
        connect(hdr, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)),
            this, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)));
        tlayout->addWidget(hdr);
        connect( qApp, SIGNAL(clockChanged(bool)), this, SLOT(slotClockChanged(bool)));

        // Events
        while ( (*it).date().dayOfWeek() == dayOrder[i] && it!=ev.end() ) {
            if(!(((*it).end().hour()==0) && ((*it).end().minute()==0) && ((*it).startDate()!=(*it).date()))) {  // Skip events ending at 00:00 starting at another day.
                DateBookWeekLstEvent *l=new DateBookWeekLstEvent(ampm,*it,weeklistviewconfig,w);
                tlayout->addWidget(l);
                connect (l, SIGNAL(editEvent(const Event&)), this, SIGNAL(editEvent(const Event&)));
                connect (l, SIGNAL(duplicateEvent(const Event &)), this, SIGNAL(duplicateEvent(const Event &)));
                connect (l, SIGNAL(removeEvent(const Event &)), this, SIGNAL(removeEvent(const Event &)));
                connect (l, SIGNAL(beamEvent(const Event &)), this, SIGNAL(beamEvent(const Event &)));
                connect (l, SIGNAL(redraw()), this, SIGNAL(redraw()));
            }
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
