#include "namespace_hack.h"
#include "datebookweeklst.h"


#include "datebook.h"

#include <qpe/datebookmonth.h>
#include <qpe/config.h>
#include <qpe/resource.h>

#include <qlayout.h>
#include <qtoolbutton.h>
#include <qtl.h>

bool calcWeek(const QDate &d, int &week, int &year,bool startOnMonday = false);

using namespace Opie::Ui;
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

DateBookWeekLstDayHdr::DateBookWeekLstDayHdr(const QDate &d, bool /* onM */,
					     QWidget* parent,
					     const char* name,
					     WFlags fl )
    : DateBookWeekLstDayHdrBase(parent, name, fl) {

	date=d;

	static const QString wdays=tr("MTWTFSSM",  "Week days");
	char day=wdays[d.dayOfWeek()-1];

    //dont use dayOfWeek() to save space !
	label->setText( QString(QObject::tr(QString(QChar(day)))) + " " +QString::number(d.day()) );

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

    emit addEvent(start,stop,"",0);
}
DateBookWeekLstEvent::DateBookWeekLstEvent(const EffectiveEvent &ev,
						int weeklistviewconfig,
						QWidget* parent,
						const char* name,
						WFlags fl ) : OClickableLabel(parent,name,fl), event(ev)
{
	// old values... lastday = "__|__", middle="   |---", Firstday="00:00",
	QString s,start,middle,end,day;

	qDebug("weeklistviewconfig=%d",weeklistviewconfig);
	if(weeklistviewconfig==NONE) {	// No times displayed.
//		start.sprintf("%.2d:%.2d-",ev.start().hour(),ev.start().minute());
//		middle.sprintf("<--->");
//		end.sprintf("-%.2d:%.2d",ev.end().hour(),ev.end().minute());
//		day.sprintf("%.2d:%.2d-%.2d:%.2d",ev.start().hour(),ev.start().minute(),ev.end().hour(),ev.end().minute());
	} else if(weeklistviewconfig==NORMAL) {	// "Normal", only display start time.
		start.sprintf("%.2d:%.2d",ev.start().hour(),ev.start().minute());
		middle.sprintf("   |---");
		end.sprintf("__|__");
		day.sprintf("%.2d:%.2d",ev.start().hour(),ev.start().minute());
	} else if(weeklistviewconfig==EXTENDED) { // Extended mode, display start and end times.
		start.sprintf("%.2d:%.2d-",ev.start().hour(),ev.start().minute());
		middle.sprintf("<--->");
		end.sprintf("-%.2d:%.2d",ev.end().hour(),ev.end().minute());
		day.sprintf("%.2d:%.2d-%.2d:%.2d",ev.start().hour(),ev.start().minute(),ev.end().hour(),ev.end().minute());
	}

	if(ev.event().type() == Event::Normal) {
		if(ev.startDate()==ev.date() && ev.endDate()==ev.date()) {	// day event.
			s=day;
		} else if(ev.startDate()==ev.date()) {	// start event.
			s=start;
		} else if(ev.endDate()==ev.date()) { // end event.
			s=end;
		} else {	// middle day.
			s=middle;
		}
	} else {
		s="";
	}
	setText(QString(s) + " " + ev.description());
	connect(this, SIGNAL(clicked()), this, SLOT(editMe()));
	setAlignment( int( QLabel::WordBreak | QLabel::AlignLeft ) );
}
void DateBookWeekLstEvent::editMe() {
	emit editEvent(event.event());
}


DateBookWeekLstView::DateBookWeekLstView(QValueList<EffectiveEvent> &ev,
					 const QDate &d, bool onM,
					 QWidget* parent,
					 const char* name, WFlags fl)
    : QWidget( parent, name, fl )
{
	Config config("DateBook");
	config.setGroup("Main");
	int weeklistviewconfig=config.readNumEntry("weeklistviewconfig", NORMAL);
	qDebug("Read weeklistviewconfig: %d",weeklistviewconfig);

	bStartOnMonday=onM;
	setPalette(white);
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	QVBoxLayout *layout = new QVBoxLayout( this );

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
	// Header
		DateBookWeekLstDayHdr *hdr=new DateBookWeekLstDayHdr(d.addDays(i-dayoffset), bStartOnMonday,this);
		connect(hdr, SIGNAL(showDate(int,int,int)), this, SIGNAL(showDate(int,int,int)));
		connect(hdr, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)),
			this, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)));
		layout->addWidget(hdr);

		// Events
		while ( (*it).date().dayOfWeek() == dayOrder[i] && it!=ev.end() ) {
			if(!(((*it).end().hour()==0) && ((*it).end().minute()==0) && ((*it).startDate()!=(*it).date()))) {	// Skip events ending at 00:00 starting at another day.
				DateBookWeekLstEvent *l=new DateBookWeekLstEvent(*it,weeklistviewconfig,this);
				layout->addWidget(l);
				connect (l, SIGNAL(editEvent(const Event&)), this, SIGNAL(editEvent(const Event&)));
			}
			it++;
		}
		layout->addItem(new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    }
}
DateBookWeekLstView::~DateBookWeekLstView(){}
void DateBookWeekLstView::keyPressEvent(QKeyEvent *e) {e->ignore();}

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
	connect (w, SIGNAL(showDate(int,int,int)), this, SIGNAL(showDate(int,int,int)));
	connect (w, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)),
		this, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)));


	w=new DateBookWeekLstView(ev2,d.addDays(7),onM,this);
	layout->addWidget(w);
	connect (w, SIGNAL(editEvent(const Event&)), this, SIGNAL(editEvent(const Event&)));
	connect (w, SIGNAL(showDate(int,int,int)), this, SIGNAL(showDate(int,int,int)));
	connect (w, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)),
		this, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)));
}

DateBookWeekLst::DateBookWeekLst( bool ap, bool onM, DateBookDB *newDB,
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
	connect (view, SIGNAL(showDate(int,int,int)), this, SIGNAL(showDate(int,int,int)));
	connect (view, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)),
		this, SIGNAL(addEvent(const QDateTime&,const QDateTime&,const QString&,const QString&)));

	scroll->addChild(view);
	view->show();
	scroll->updateScrollBars();
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
