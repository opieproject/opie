#include "datebookweeklstheader.h"

#include <qpe/resource.h>
#include <qpe/datebookmonth.h>

#include <qtoolbutton.h>
#include <qlayout.h>

/* implenented in datebookweek.cpp - HELL */
bool calcWeek(const QDate &d, int &week, int &year,bool startOnMonday = false);

DateBookWeekLstHeader::DateBookWeekLstHeader(bool onM, QWidget* parent, const char* name, WFlags fl)
    : DateBookWeekLstHeaderBase(parent, name, fl)
{
    setBackgroundMode( PaletteButton );
    labelDate->setBackgroundMode( PaletteButton );
    forwardweek->setBackgroundMode( PaletteButton );
    forwardweek->setPixmap( Resource::loadPixmap("forward") );
    forwardmonth->setBackgroundMode( PaletteButton );
    forwardmonth->setPixmap( Resource::loadPixmap("fastforward") );
    backweek->setBackgroundMode( PaletteButton );
    backweek->setPixmap( Resource::loadPixmap("back") );
    backmonth->setBackgroundMode( PaletteButton );
    backmonth->setPixmap( Resource::loadPixmap("fastback") );
    DateBookWeekLstHeaderBaseLayout->setSpacing(0);
    DateBookWeekLstHeaderBaseLayout->setMargin(0);
    //setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding));
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));

    connect(backmonth, SIGNAL(clicked()), this, SLOT(prevMonth()));
    connect(backweek, SIGNAL(clicked()), this, SLOT(prevWeek()));
    connect(forwardweek, SIGNAL(clicked()), this, SLOT(nextWeek()));
    connect(forwardmonth, SIGNAL(clicked()), this, SLOT(nextMonth()));
    connect(labelDate, SIGNAL(clicked()), this, SLOT(pickDate()));
    connect(dbl, SIGNAL(toggled(bool)), this, SIGNAL(setDbl(bool)));
    bStartOnMonday=onM;
}
DateBookWeekLstHeader::~DateBookWeekLstHeader(){}

void DateBookWeekLstHeader::setDate(const QDate &d) {
    int year,week,dayofweek;
    date=d;
    dayofweek=d.dayOfWeek();
    if(bStartOnMonday)
        dayofweek--;
    else if( dayofweek == 7 )
        /* we already have the right day -7 would lead to the same week */
        dayofweek = 0;

    date=date.addDays(-dayofweek);

    calcWeek(date,week,year,bStartOnMonday);
    QDate start=date;
    QDate stop=start.addDays(6);
    labelDate->setText( QString::number(start.day()) + "." +
            Calendar::nameOfMonth( start.month() ) + "-" +
            QString::number(stop.day()) + "." +
                                                Calendar::nameOfMonth( stop.month()) +" ("+
            tr("w")+":"+QString::number( week ) +")");
    date = d; // bugfix: 0001126 - date has to be the selected date, not monday!
    emit dateChanged(date);
}

void DateBookWeekLstHeader::pickDate() {
    static QPopupMenu *m1 = 0;
    static DateBookMonth *picker = 0;
    if ( !m1 ) {
        m1 = new QPopupMenu( this );
        picker = new DateBookMonth( m1, 0, TRUE );
        m1->insertItem( picker );
        connect( picker, SIGNAL( dateClicked(int,int,int) ),this, SLOT( setDate(int,int,int) ) );
        //connect( m1, SIGNAL( aboutToHide() ),
        //this, SLOT( gotHide() ) );
    }
    picker->setDate( date.year(), date.month(), date.day() );
    m1->popup(mapToGlobal(labelDate->pos()+QPoint(0,labelDate->height())));
    picker->setFocus();
}
void DateBookWeekLstHeader::setDate(int y, int m, int d) {
    setDate(QDate(y,m,d));
}

void DateBookWeekLstHeader::nextWeek() {
    setDate(date.addDays(7));
}
void DateBookWeekLstHeader::prevWeek() {
    setDate(date.addDays(-7));
}
void DateBookWeekLstHeader::nextMonth()
{
    setDate(date.addDays(28));
}
void DateBookWeekLstHeader::prevMonth()
{
    setDate(date.addDays(-28));
}
