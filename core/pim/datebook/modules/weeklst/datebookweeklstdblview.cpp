#include "datebookweeklstdblview.h"
#include "datebookweeklstview.h"

#include <qlayout.h>

DateBookWeekLstDblView::DateBookWeekLstDblView(QValueList<EffectiveEvent> &ev1,
                           QValueList<EffectiveEvent> &ev2,
                           QDate &d, bool onM,
                           QWidget* parent,
                           const char* name, WFlags fl)
    : QWidget( parent, name, fl )
{
    QHBoxLayout *layout = new QHBoxLayout( this );

    DateBookWeekLstView *w=new DateBookWeekLstView(ev1,d,onM,this);
    layout->addWidget(w);
    connect (w, SIGNAL(editEvent(const Event&)), this, SIGNAL(editEvent(const Event&)));
    connect (w, SIGNAL(duplicateEvent(const Event &)), this, SIGNAL(duplicateEvent(const Event &)));
    connect (w, SIGNAL(removeEvent(const Event &)), this, SIGNAL(removeEvent(const Event &)));
    connect (w, SIGNAL(beamEvent(const Event &)), this, SIGNAL(beamEvent(const Event &)));
    connect (w, SIGNAL(redraw()), this, SIGNAL(redraw()));
    connect (w, SIGNAL(showDate(int,int,int)), this, SIGNAL(showDate(int,int,int)));
    connect (w, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)),
        this, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)));


    w=new DateBookWeekLstView(ev2,d.addDays(7),onM,this);
    layout->addWidget(w);
    connect (w, SIGNAL(editEvent(const Event&)), this, SIGNAL(editEvent(const Event&)));
    connect (w, SIGNAL(duplicateEvent(const Event &)), this, SIGNAL(duplicateEvent(const Event &)));
    connect (w, SIGNAL(removeEvent(const Event &)), this, SIGNAL(removeEvent(const Event &)));
    connect (w, SIGNAL(beamEvent(const Event &)), this, SIGNAL(beamEvent(const Event &)));
    connect (w, SIGNAL(redraw()), this, SIGNAL(redraw()));
    connect (w, SIGNAL(showDate(int,int,int)), this, SIGNAL(showDate(int,int,int)));
    connect (w, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)),
        this, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)));
}

DateBookWeekLstDblView::~DateBookWeekLstDblView()
{
}
