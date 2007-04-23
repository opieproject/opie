#include "datebookweeklstdblview.h"
#include "datebookweeklstview.h"

#include <qlayout.h>

DateBookWeekLstDblView::DateBookWeekLstDblView(QValueList<EffectiveEvent> &ev1,
                           QValueList<EffectiveEvent> &ev2,
                           QDate &d, bool onM, bool showAmPm,
                           QWidget* parent,
                           const char* name, WFlags fl)
    : QWidget( parent, name, fl ), ampm( showAmPm )
{
    m_MainLayout = new QHBoxLayout( this );

    leftView = 0;
    rightView = 0;
    setEvents(ev1,ev2,d,onM);
}

DateBookWeekLstDblView::DateBookWeekLstDblView(QValueList<EffectiveEvent> &ev1,
                           QDate &d, bool onM, bool showAmPm,
                           QWidget* parent,
                           const char* name, WFlags fl)
    : QWidget( parent, name, fl ), ampm( showAmPm )
{
    m_MainLayout = new QHBoxLayout( this );

    leftView = 0;
    rightView = 0;
    setEvents(ev1,d,onM);
}

/* setting the variant with both views */
void DateBookWeekLstDblView::setEvents(QValueList<EffectiveEvent> &ev1,QValueList<EffectiveEvent> &ev2,QDate &d, bool onM)
{
    setUpdatesEnabled(false);
    if (!leftView) {
        leftView=new DateBookWeekLstView(ev1, d, onM, ampm, this);
        m_MainLayout->addWidget(leftView);
        connect (leftView, SIGNAL(editEvent(const Event&)), this, SIGNAL(editEvent(const Event&)));
        connect (leftView, SIGNAL(duplicateEvent(const Event &)), this, SIGNAL(duplicateEvent(const Event &)));
        connect (leftView, SIGNAL(removeEvent(const Event &)), this, SIGNAL(removeEvent(const Event &)));
        connect (leftView, SIGNAL(beamEvent(const Event &)), this, SIGNAL(beamEvent(const Event &)));
        connect (leftView, SIGNAL(redraw()), this, SIGNAL(redraw()));
        connect (leftView, SIGNAL(showDate(int,int,int)), this, SIGNAL(showDate(int,int,int)));
        connect (leftView, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)),
        this, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)));

    } else {
        leftView->hide();
        leftView->setEvents(ev1,d,onM);
    }

    if (!rightView) {
        rightView=new DateBookWeekLstView(ev2, d.addDays(7), onM, ampm, this);
        m_MainLayout->addWidget(rightView);
        connect (rightView, SIGNAL(editEvent(const Event&)), this, SIGNAL(editEvent(const Event&)));
        connect (rightView, SIGNAL(duplicateEvent(const Event &)), this, SIGNAL(duplicateEvent(const Event &)));
        connect (rightView, SIGNAL(removeEvent(const Event &)), this, SIGNAL(removeEvent(const Event &)));
        connect (rightView, SIGNAL(beamEvent(const Event &)), this, SIGNAL(beamEvent(const Event &)));
        connect (rightView, SIGNAL(redraw()), this, SIGNAL(redraw()));
        connect (rightView, SIGNAL(showDate(int,int,int)), this, SIGNAL(showDate(int,int,int)));
        connect (rightView, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)),
            this, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)));
    } else {
        rightView->hide();
        rightView->setEvents(ev2,d.addDays(7),onM);
    }

    leftView->show();
    rightView->show();
    setUpdatesEnabled(true);
}

void DateBookWeekLstDblView::setEvents(QValueList<EffectiveEvent> &ev1,QDate &d, bool onM)
{
    if (!leftView) {
        leftView=new DateBookWeekLstView(ev1, d, onM, ampm, this);
        m_MainLayout->addWidget(leftView);
        connect (leftView, SIGNAL(editEvent(const Event&)), this, SIGNAL(editEvent(const Event&)));
        connect (leftView, SIGNAL(duplicateEvent(const Event &)), this, SIGNAL(duplicateEvent(const Event &)));
        connect (leftView, SIGNAL(removeEvent(const Event &)), this, SIGNAL(removeEvent(const Event &)));
        connect (leftView, SIGNAL(beamEvent(const Event &)), this, SIGNAL(beamEvent(const Event &)));
        connect (leftView, SIGNAL(redraw()), this, SIGNAL(redraw()));
        connect (leftView, SIGNAL(showDate(int,int,int)), this, SIGNAL(showDate(int,int,int)));
        connect (leftView, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)),
            this, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)));
    } else {
        leftView->hide();
        leftView->setEvents(ev1,d,onM);
    }
    leftView->show();

    if (rightView) {
        rightView->hide();
    }
}

void DateBookWeekLstDblView::setRightEvents(QValueList<EffectiveEvent> &ev1,QDate &d, bool onM)
{
    if (!rightView) {
        rightView=new DateBookWeekLstView(ev1, d, onM, ampm, this);
        m_MainLayout->addWidget(rightView);
        connect (leftView, SIGNAL(editEvent(const Event&)), this, SIGNAL(editEvent(const Event&)));
        connect (leftView, SIGNAL(duplicateEvent(const Event &)), this, SIGNAL(duplicateEvent(const Event &)));
        connect (leftView, SIGNAL(removeEvent(const Event &)), this, SIGNAL(removeEvent(const Event &)));
        connect (leftView, SIGNAL(beamEvent(const Event &)), this, SIGNAL(beamEvent(const Event &)));
        connect (leftView, SIGNAL(redraw()), this, SIGNAL(redraw()));
        connect (leftView, SIGNAL(showDate(int,int,int)), this, SIGNAL(showDate(int,int,int)));
        connect (leftView, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)),
            this, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)));
    } else {
        rightView->hide();
        rightView->setEvents(ev1,d,onM);
    }
    rightView->show();
}

bool DateBookWeekLstDblView::toggleDoubleView(bool how)
{
    if (rightView) {
        if (how) rightView->show();
        else rightView->hide();
        return true;
    }
    return false;
}

DateBookWeekLstDblView::~DateBookWeekLstDblView()
{
}
