#include "datebookweeklstdblview.h"
#include "datebookweeklstview.h"

#include <qlayout.h>

using namespace Opie;
using namespace Opie::Datebook;

DateBookWeekLstDblView::DateBookWeekLstDblView(WeekLstView *view,
                           OPimOccurrence::List &ev1,
                           OPimOccurrence::List &ev2,
                           QDate &d, bool onM, bool showAmPm,
                           QWidget* parent,
                           const char* name, WFlags fl)
    : QWidget( parent, name, fl ), ampm( showAmPm )
{
    m_MainLayout = new QHBoxLayout( this );

    weekLstView = view;
    leftView = 0;
    rightView = 0;
    setEvents(ev1,ev2,d,onM);
}

DateBookWeekLstDblView::DateBookWeekLstDblView(WeekLstView *view,
                           OPimOccurrence::List &ev1,
                           QDate &d, bool onM, bool showAmPm,
                           QWidget* parent,
                           const char* name, WFlags fl)
    : QWidget( parent, name, fl ), ampm( showAmPm )
{
    m_MainLayout = new QHBoxLayout( this );

    weekLstView = view;
    leftView = 0;
    rightView = 0;
    setEvents(ev1,d,onM);
}

/* setting the variant with both views */
void DateBookWeekLstDblView::setEvents(OPimOccurrence::List &ev1,OPimOccurrence::List &ev2,QDate &d, bool onM)
{
    setUpdatesEnabled(false);
    if (!leftView) {
        leftView = new DateBookWeekLstView(ev1, weekLstView, d, onM, ampm, this);
        m_MainLayout->addWidget(leftView);
        connect (leftView, SIGNAL(redraw()), this, SIGNAL(redraw()));
        connect (leftView, SIGNAL(showDate(int,int,int)), this, SIGNAL(showDate(int,int,int)));
        connect (leftView, SIGNAL(addEvent(const QDateTime&,const QDateTime&)),
        this, SIGNAL(addEvent(const QDateTime&,const QDateTime&)));

    } else {
        leftView->hide();
        leftView->setEvents(ev1,d,onM);
    }

    if (!rightView) {
        rightView = new DateBookWeekLstView(ev2, weekLstView, d.addDays(7), onM, ampm, this);
        m_MainLayout->addWidget(rightView);
        connect (rightView, SIGNAL(redraw()), this, SIGNAL(redraw()));
        connect (rightView, SIGNAL(showDate(int,int,int)), this, SIGNAL(showDate(int,int,int)));
        connect (rightView, SIGNAL(addEvent(const QDateTime&,const QDateTime&)),
            this, SIGNAL(addEvent(const QDateTime&,const QDateTime&)));
    } else {
        rightView->hide();
        rightView->setEvents(ev2,d.addDays(7),onM);
    }

    leftView->show();
    rightView->show();
    setUpdatesEnabled(true);
}

void DateBookWeekLstDblView::setEvents(OPimOccurrence::List &ev1,QDate &d, bool onM)
{
    if (!leftView) {
        leftView = new DateBookWeekLstView(ev1, weekLstView, d, onM, ampm, this);
        m_MainLayout->addWidget(leftView);
        connect (leftView, SIGNAL(redraw()), this, SIGNAL(redraw()));
        connect (leftView, SIGNAL(showDate(int,int,int)), this, SIGNAL(showDate(int,int,int)));
        connect (leftView, SIGNAL(addEvent(const QDateTime&,const QDateTime&)),
            this, SIGNAL(addEvent(const QDateTime&,const QDateTime&)));
    } else {
        leftView->hide();
        leftView->setEvents(ev1,d,onM);
    }
    leftView->show();

    if (rightView) {
        rightView->hide();
    }
}

void DateBookWeekLstDblView::setRightEvents(OPimOccurrence::List &ev1,QDate &d, bool onM)
{
    if (!rightView) {
        rightView = new DateBookWeekLstView(ev1, weekLstView, d, onM, ampm, this);
        m_MainLayout->addWidget(rightView);
        connect (leftView, SIGNAL(redraw()), this, SIGNAL(redraw()));
        connect (leftView, SIGNAL(showDate(int,int,int)), this, SIGNAL(showDate(int,int,int)));
        connect (leftView, SIGNAL(addEvent(const QDateTime&,const QDateTime&)),
            this, SIGNAL(addEvent(const QDateTime&,const QDateTime&)));
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
