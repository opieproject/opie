#include "namespace_hack.h"
#include "datebookweeklst.h"
#include "datebookweeklstheader.h"
#include "datebookweeklstview.h"
#include "datebookweeklstdblview.h"

#include "datebook.h"

#include <opie2/odebug.h>

#include <qpe/datebookmonth.h>
#include <qpe/config.h>
#include <qpe/resource.h>

#include <qlayout.h>
#include <qtoolbutton.h>

using namespace Opie::Ui;

DateBookWeekLst::DateBookWeekLst( bool ap, bool onM, DateBookDBHoliday *newDB,
                  QWidget *parent,
                  const char *name )
    : QWidget( parent, name ),
      db( newDB ),
      startTime( 0 ),
      ampm( ap ),
      bStartOnMonday(onM)
{
    setFocusPolicy(StrongFocus);
    layout = new QVBoxLayout( this );
    layout->setMargin(0);

    header=new DateBookWeekLstHeader(onM, this);
    layout->addWidget( header );
    connect(header, SIGNAL(dateChanged(QDate&)), this, SLOT(dateChanged(QDate&)));
    connect(header, SIGNAL(setDbl(bool)), this, SLOT(setDbl(bool)));

    scroll=new QScrollView(this);
    scroll->setResizePolicy(QScrollView::AutoOneFit);
    layout->addWidget(scroll);

    view=NULL;
    Config config("DateBook");
    config.setGroup("Main");
    dbl=config.readBoolEntry("weeklst_dbl", false);
    header->dbl->setOn(dbl);
}
DateBookWeekLst::~DateBookWeekLst(){
    Config config("DateBook");
    config.setGroup("Main");
    config.writeEntry("weeklst_dbl", dbl);
}

void DateBookWeekLst::setDate(const QDate &d) {
    bdate=d;
    header->setDate(d);
}

void DateBookWeekLst::setDbl(bool on) {
    dbl=on;
    redraw();
}
void DateBookWeekLst::redraw() {getEvents();}

QDate DateBookWeekLst::date() {
    return bdate;
}

// return the date at the beginning of the week...
// copied from DateBookWeek
QDate DateBookWeekLst::weekDate() const
{
    QDate d=bdate;

    // Calculate offset to first day of week.
    int dayoffset=d.dayOfWeek();
    if(bStartOnMonday) dayoffset--;
    else if( dayoffset == 7 )
        dayoffset = 0;

    return d.addDays(-dayoffset);
}

void DateBookWeekLst::getEvents() {
    QDate start = weekDate(); //date();
    QDate stop = start.addDays(6);
    QValueList<EffectiveEvent> el = db->getEffectiveEvents(start, stop);

    setUpdatesEnabled(false);
    if (view) delete view;
    if (dbl) {
        QDate start2=start.addDays(7);
        stop=start2.addDays(6);
        QValueList<EffectiveEvent> el2 = db->getEffectiveEvents(start2, stop);
        view=new DateBookWeekLstDblView(el,el2,start,bStartOnMonday,scroll);
    } else {
        view=new DateBookWeekLstView(el,start,bStartOnMonday,scroll);
    }

    connect (view, SIGNAL(editEvent(const Event&)), this, SIGNAL(editEvent(const Event&)));
    connect (view, SIGNAL(duplicateEvent(const Event &)), this, SIGNAL(duplicateEvent(const Event &)));
    connect (view, SIGNAL(removeEvent(const Event &)), this, SIGNAL(removeEvent(const Event &)));
    connect (view, SIGNAL(beamEvent(const Event &)), this, SIGNAL(beamEvent(const Event &)));
    connect (view, SIGNAL(redraw()), this, SLOT(redraw()));
    connect (view, SIGNAL(showDate(int,int,int)), this, SIGNAL(showDate(int,int,int)));
    connect (view, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)),
        this, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)));

    scroll->addChild(view);
    view->show();
    scroll->updateScrollBars();
    setUpdatesEnabled(true);
}

void DateBookWeekLst::dateChanged(QDate &newdate) {
    bdate=newdate;
    getEvents();
}

void DateBookWeekLst::keyPressEvent(QKeyEvent *e)
{
    switch(e->key()) {
        case Key_Up:
            scroll->scrollBy(0, -20);
            break;
        case Key_Down:
            scroll->scrollBy(0, 20);
            break;
        case Key_Left:
            header->prevWeek();
            break;
        case Key_Right:
            header->nextWeek();
            break;
        default:
            e->ignore();
    }
}
