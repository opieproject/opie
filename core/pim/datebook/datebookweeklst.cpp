#include "datebookweeklst.h"

#include "datebookweekheaderimpl.h"

#include <qpe/calendar.h>
#include <qpe/datebookdb.h>
#include <qpe/event.h>
#include <qpe/qpeapplication.h>
#include <qpe/timestring.h>
#include <qpe/datebookmonth.h>

#include <qdatetime.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qstyle.h>
#include <qtoolbutton.h>
#include <qvbox.h>
#include <qsizepolicy.h> 
#include <qabstractlayout.h> 
#include <qtl.h>

bool calcWeek(const QDate &d, int &week, int &year,
	      bool startOnMonday = false);

DateBookWeekLstHeader::DateBookWeekLstHeader(bool onM, QWidget* parent,  
					     const char* name, WFlags fl)
    : DateBookWeekLstHeaderBase(parent, name, fl)
{
    setBackgroundMode( PaletteButton );
    labelDate->setBackgroundMode( PaletteButton );
    labelWeek->setBackgroundMode( PaletteButton );
    forward->setBackgroundMode( PaletteButton );
    back->setBackgroundMode( PaletteButton );
    DateBookWeekLstHeaderBaseLayout->setSpacing(0);
    DateBookWeekLstHeaderBaseLayout->setMargin(0);
    //setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding));
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));

    connect(back, SIGNAL(clicked()), this, SLOT(prevWeek()));
    connect(forward, SIGNAL(clicked()), this, SLOT(nextWeek()));
    connect(labelWeek, SIGNAL(clicked()), this, SLOT(pickDate()));
    onMonday=onM;
}
DateBookWeekLstHeader::~DateBookWeekLstHeader(){}
void DateBookWeekLstHeader::setDate(QDate d) {
    date=d;

    int year,week;
    calcWeek(d,week,year,onMonday);
    labelWeek->setText("W: " + QString::number(week));
	
    QDate start=date;
    QDate stop=start.addDays(6);
    labelDate->setText( QString::number(start.day()) + " " + 
			start.monthName(start.month()) + " - " +
			QString::number(stop.day()) + " " + 
			start.monthName(stop.month()) );
    emit dateChanged(year,week);
}
void DateBookWeekLstHeader::pickDate() {
    static QPopupMenu *m1 = 0;
    static DateBookMonth *picker = 0;
    if ( !m1 ) {
	m1 = new QPopupMenu( this );
	picker = new DateBookMonth( m1, 0, TRUE );
	m1->insertItem( picker );
	connect( picker, SIGNAL( dateClicked( int, int, int ) ),
		 this, SLOT( setDate( int, int, int ) ) );
	//connect( m1, SIGNAL( aboutToHide() ),
	//this, SLOT( gotHide() ) );
    }
    picker->setDate( date.year(), date.month(), date.day() );
    m1->popup(mapToGlobal(labelWeek->pos()+QPoint(0,labelWeek->height())));
    picker->setFocus();
}
void DateBookWeekLstHeader::setDate(int y, int m, int d) {
  QDate new_date(y,m,d);
  setDate(new_date);
}

void DateBookWeekLstHeader::nextWeek() {
    setDate(date.addDays(7));
}
void DateBookWeekLstHeader::prevWeek() {
    setDate(date.addDays(-7));
}

DateBookWeekLstDayHdr::DateBookWeekLstDayHdr(const QDate &d, bool onM,
					     QWidget* parent = 0, 
					     const char* name = 0, 
					     WFlags fl = 0 ) 
    : DateBookWeekLstDayHdrBase(parent, name, fl) {

    date=d;

    static const char *wdays="MTWTFSS";
    char day=wdays[d.dayOfWeek()-1];

    label->setText( QString(QChar(day)) + " " + 
		    QString::number(d.day()) );
    add->setText("+");

    if (d == QDate::currentDate()) {
	QPalette pal=label->palette();
	pal.setColor(QColorGroup::Foreground, QColor(0,0,255));
	label->setPalette(pal);
	
	/*
	  QFont f=label->font();
	  f.setItalic(true);
	  label->setFont(f);
	  label->setPalette(QPalette(QColor(0,0,255),label->backgroundColor()));
	*/
    } else if (d.dayOfWeek() == 7) { // FIXME: Match any holiday
	QPalette pal=label->palette();
	pal.setColor(QColorGroup::Foreground, QColor(255,0,0));
	label->setPalette(pal);
    }


    connect (label, SIGNAL(clicked()), this, SLOT(showDay()));
    connect (add, SIGNAL(clicked()), this, SLOT(newEvent()));
}

void DateBookWeekLstDayHdr::showDay() {
    emit showDate(date.year(), date.month(), date.day());
}
void DateBookWeekLstDayHdr::newEvent() {
    QDateTime start, stop;
    start=stop=date;
    start.setTime(QTime(10,0));
    stop.setTime(QTime(12,0));

    emit addEvent(start,stop,"");
}
DateBookWeekLstEvent::DateBookWeekLstEvent(const EffectiveEvent &ev, 
					   QWidget* parent = 0, 
					   const char* name = 0, 
					   WFlags fl = 0) :
    ClickableLabel(parent,name,fl),
    event(ev)
{
    char s[10];
    if ( ev.startDate() != ev.date() ) { // multiday event (not first day)
      if ( ev.endDate() == ev.date() ) { // last day
	strcpy(s, "__|__");
      } else {
	strcpy(s, "    |---");
      }
    } else {
      sprintf(s,"%.2d:%.2d",ev.start().hour(),ev.start().minute());
    }
    setText(QString(s) + " " + ev.description());
    connect(this, SIGNAL(clicked()), this, SLOT(editMe()));
}
void DateBookWeekLstEvent::editMe() {
    emit editEvent(event.event());
}


DateBookWeekLstView::DateBookWeekLstView(QValueList<EffectiveEvent> &ev, 
					 QDate &d, bool onM,
					 QWidget* parent,  
					 const char* name, WFlags fl)
    : QWidget( parent, name, fl )
{
    onMonday=onM;
    setPalette(white);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

    QVBoxLayout *layout = new QVBoxLayout( this );
	
    qBubbleSort(ev);
    QValueListIterator<EffectiveEvent> it;
    it=ev.begin();

    int dayOrder[7];
    if (onMonday) 
	for (int d=0; d<7; d++) dayOrder[d]=d+1;
    else {
	for (int d=0; d<7; d++) dayOrder[d]=d;
	dayOrder[0]=7;
    }

    for (int i=0; i<7; i++) {
	// Header
	DateBookWeekLstDayHdr *hdr=new DateBookWeekLstDayHdr(d.addDays(i),
							     onMonday,this);
	connect(hdr, SIGNAL(showDate(int,int,int)), 
		this, SIGNAL(showDate(int,int,int)));
	connect(hdr, SIGNAL(addEvent(const QDateTime &, 
				     const QDateTime &,
				     const QString &)), 
		this, SIGNAL(addEvent(const QDateTime &, 
				      const QDateTime &,
				      const QString &)));
	layout->addWidget(hdr);
		
	// Events
	while ( (*it).date().dayOfWeek() == dayOrder[i] && it!=ev.end() ) {
	    DateBookWeekLstEvent *l=new DateBookWeekLstEvent(*it,this);
	    layout->addWidget(l);
	    connect (l, SIGNAL(editEvent(const Event &)),
		     this, SIGNAL(editEvent(const Event &)));
	    it++;
	}
	
	layout->addItem(new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    }
}
DateBookWeekLstView::~DateBookWeekLstView(){}
void DateBookWeekLstView::keyPressEvent(QKeyEvent *e) {e->ignore();}

DateBookWeekLst::DateBookWeekLst( bool ap, bool onM, DateBookDB *newDB, 
				  QWidget *parent, 
				  const char *name )
    : QWidget( parent, name ),
      db( newDB ),
      startTime( 0 ),
      ampm( ap ),
      onMonday(onM)
{
    setFocusPolicy(StrongFocus);
    layout = new QVBoxLayout( this );
    layout->setMargin(0);

    header=new DateBookWeekLstHeader(onM, this);
    layout->addWidget( header );
    connect(header, SIGNAL(dateChanged(int,int)), this, SLOT(dateChanged(int,int)));

    scroll=new QScrollView(this);
    //scroll->setVScrollBarMode(QScrollView::AlwaysOn);
    //scroll->setHScrollBarMode(QScrollView::AlwaysOff);
    scroll->setResizePolicy(QScrollView::AutoOneFit);
    layout->addWidget(scroll);

    view=NULL;
}


void DateBookWeekLst::setDate( QDate &d ) {
    int w,y;
    calcWeek(d,w,y,onMonday);
    year=y;
    _week=w;
    header->setDate(date());
}
void DateBookWeekLst::redraw() {getEvents();}

QDate DateBookWeekLst::date() const {
    QDate d;
    d.setYMD(year,1,1);

    int dow= d.dayOfWeek();
    if (!onMonday)
	if (dow==7) dow=1;
	else dow++;
	
    d=d.addDays( (_week-1)*7 - dow + 1 );
    return d;
}

void DateBookWeekLst::getEvents() {
    QDate start = date();
    QDate stop = start.addDays(6);
    QValueList<EffectiveEvent> el = db->getEffectiveEvents(start, stop);

    if (view) delete view;
    view=new DateBookWeekLstView(el,start,onMonday,scroll);

    connect (view, SIGNAL(editEvent(const Event &)),
	     this, SIGNAL(editEvent(const Event &)));
    connect (view, SIGNAL(showDate(int,int,int)),
	     this, SIGNAL(showDate(int,int,int)));
    connect (view, SIGNAL(addEvent(const QDateTime &, const QDateTime &,
				   const QString &)),
	     this, SIGNAL(addEvent(const QDateTime &, const QDateTime &,
				   const QString &)));

    scroll->addChild(view);
    view->show();
    scroll->updateScrollBars();
}

void DateBookWeekLst::dateChanged(int y, int w) {
    year=y;
    _week=w;
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

