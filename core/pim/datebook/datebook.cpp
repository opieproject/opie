/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** $Id: datebook.cpp,v 1.13 2002-06-27 12:42:37 zecke Exp $
**
**********************************************************************/

#define QTOPIA_INTERNAL_FD

#include "datebook.h"
#include "datebookday.h"
#include "datebooksettings.h"
#include "datebookweek.h"
#include "datebookweeklst.h"
#include "dateentryimpl.h"

#include <qpe/datebookmonth.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/qpedebug.h>
#include <qpe/event.h>
#include <qpe/finddialog.h>
#include <qpe/ir.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpemessagebox.h>
#include <qpe/resource.h>
#include <qpe/sound.h>
#include <qpe/timestring.h>
#include <qpe/qpetoolbar.h>
#include <qpe/tzselect.h>
#include <qpe/xmlreader.h>

#include <qaction.h>
#include <qcopchannel_qws.h>
#include <qdatetime.h>
#include <qdialog.h>
#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qtextcodec.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qtl.h>
#include <qwidgetstack.h>
#include <qwindowsystem_qws.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>

#define DAY 1
#define WEEK 2
#define WEEKLST 4
#define MONTH 3


DateBook::DateBook( QWidget *parent, const char *, WFlags f )
    : QMainWindow( parent, "datebook", f ),
      aPreset( FALSE ),
      presetTime( -1 ),
      startTime( 8 ), // an acceptable default
      rowStyle( 0 ),
      bJumpToCurTime(FALSE),
      syncing(FALSE),
      inSearch(FALSE),
      alarmCounter(0)
{
    bool needEvilHack= false; // if we need an Evil Hack
    QTime t;
    t.start();
    db = new DateBookDBHack;
    qDebug("loading db t=%d", t.elapsed() );
    loadSettings();
    setCaption( tr("Calendar") );
    setIcon( Resource::loadPixmap( "datebook_icon" ) );

    setToolBarsMovable( FALSE );

    views = new QWidgetStack( this );
    setCentralWidget( views );

    dayView = 0;
    weekView = 0;
    weekLstView = 0;
    monthView = 0;

    QPEToolBar *bar = new QPEToolBar( this );
    bar->setHorizontalStretchable( TRUE );

    QPEMenuBar *mb = new QPEMenuBar( bar );
    mb->setMargin( 0 );

    QPEToolBar *sub_bar = new QPEToolBar(this);

    QPopupMenu *view = new QPopupMenu( this );
    QPopupMenu *settings = new QPopupMenu( this );

    mb->insertItem( tr( "View" ), view );
    mb->insertItem( tr( "Settings" ), settings );

    QActionGroup *g = new QActionGroup( this );
    g->setExclusive( TRUE );

    QAction *a = new QAction( tr( "New" ), Resource::loadPixmap( "new" ),
                              QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( fileNew() ) );
    a->addTo( sub_bar );

    a = new QAction( tr( "Today" ), Resource::loadPixmap( "to_day" ),  QString::null, 0, g, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( slotToday() ) );
    a->addTo( sub_bar );
    a->addTo( view );

    a = new QAction( tr( "Day" ), Resource::loadPixmap( "day" ), QString::null, 0, g, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( viewDay() ) );
    a->addTo( sub_bar );
    a->addTo( view );
    a->setToggleAction( TRUE );
    a->setOn( TRUE );
    dayAction = a;

    a = new QAction( tr( "Week" ), Resource::loadPixmap( "week" ), QString::null, 0, g, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( viewWeek() ) );
    a->addTo( sub_bar );
    a->addTo( view );
    a->setToggleAction( TRUE );
    weekAction = a;

    a = new QAction( tr( "WeekLst" ), Resource::loadPixmap( "weeklst" ), QString::null, 0, g, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( viewWeekLst() ) );
    a->addTo( sub_bar );
    a->addTo( view );
    a->setToggleAction( TRUE );
    weekLstAction = a;

    a = new QAction( tr( "Month" ), Resource::loadPixmap( "month" ), QString::null, 0, g, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( viewMonth() ) );
    a->addTo( sub_bar );
    a->addTo( view );
    a->setToggleAction( TRUE );
    monthAction = a;

    a = new QAction( tr( "Find" ), Resource::loadPixmap( "mag" ), QString::null, 0, g, 0 );
    connect( a, SIGNAL(activated()), this, SLOT(slotFind()) );
    a->addTo( sub_bar );

    a = new QAction( tr( "Alarm and Start Time..." ), QString::null, 0, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( slotSettings() ) );
    a->addTo( settings );

    QPopupMenu *default_view = new QPopupMenu(this);
    settings->insertItem( tr( "Default View" ),default_view );
    default_view->setCheckable(TRUE);

    Config config("DateBook");
    config.setGroup("Main");
    int current=config.readNumEntry("defaultview", DAY);

    QActionGroup *ag = new QActionGroup(this);
    a = new QAction( tr( "Day" ), QString::null, 0, 0, 0, true );
    if (current==DAY) a->setOn(true), viewDay();
    ag->insert(a);
    a = new QAction( tr( "Week" ), QString::null, 0, 0, 0, true );
    if (current==WEEK) a->setOn(true), /*viewWeek(),*/ needEvilHack = true;
    ag->insert(a);
    a = new QAction( tr( "WeekLst" ), QString::null, 0, 0, 0, true );
    if (current==WEEKLST) a->setOn(true), viewWeekLst();
    ag->insert(a);
    a = new QAction( tr( "Month" ), QString::null, 0, 0, 0, true );
    if (current==MONTH) a->setOn(true), viewMonth();
    ag->insert(a);

    ag->addTo(default_view);
    connect(ag, SIGNAL( selected ( QAction * ) ),
	    this, SLOT( newDefaultView(QAction *) )
	    );

    connect( qApp, SIGNAL(clockChanged(bool)),
             this, SLOT(changeClock(bool)) );
    connect( qApp, SIGNAL(weekChanged(bool)),
             this, SLOT(changeWeek(bool)) );

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    connect( qApp, SIGNAL(appMessage(const QCString&, const QByteArray&)),
	     this, SLOT(appMessage(const QCString&, const QByteArray&)) );
#endif

    // listen on QPE/System
#if defined(Q_WS_QWS)
#if !defined(QT_NO_COP)
    QCopChannel *channel = new QCopChannel( "QPE/System", this );
    connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
	     this, SLOT(receive(const QCString&, const QByteArray&)) );
    channel = new QCopChannel( "QPE/Datebook", this );
    connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
	     this, SLOT(receive(const QCString&, const QByteArray&)) );
    qDebug("olle\n");
#endif
#endif

    qDebug("done t=%d", t.elapsed() );
    
    /*
     *  Here is a problem description:
     *  When Weekview is the default view
     *  a DateBookWeekView get's created
     *  redraw() get's called. So what?
     *  Remember that we're still in the c'tor
     *  and no final layout has happened? Ok
     *  now all Events get arranged. Their x
     *  position get's determined by a QHeader
     *  position. But the QHeader isn't layouted or
     *  at the right position. redraw() is a slot
     *  so we'll call it then via a singleShot
     *  from view()
     */
    if( needEvilHack ){
	QTimer::singleShot( 500, this, SLOT(viewWeek()) );
    }
}

void DateBook::receive( const QCString &msg, const QByteArray &data )
{
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "timeChange(QString)" ) {
	// update active view!
	if ( dayAction->isOn() )
	    viewDay();
	else if ( weekAction->isOn() )
	    viewWeek();
	else if ( monthAction->isOn() )
	    viewMonth();
    }
    else if (msg == "editEvent(int)") {
	int uid;
	stream >> uid;
	Event e=db->eventByUID(uid);
	editEvent(e);
    }
}

DateBook::~DateBook()
{
}

void DateBook::slotSettings()
{
    DateBookSettings frmSettings( ampm, this );
    frmSettings.setStartTime( startTime );
    frmSettings.setAlarmPreset( aPreset, presetTime );
    frmSettings.setJumpToCurTime( bJumpToCurTime );
    frmSettings.setRowStyle( rowStyle );
#if defined (Q_WS_QWS) || defined(_WS_QWS_)
    frmSettings.showMaximized();
#endif

    if ( frmSettings.exec() ) {
	aPreset = frmSettings.alarmPreset();
	presetTime = frmSettings.presetTime();
	startTime = frmSettings.startTime();
	bJumpToCurTime = frmSettings.jumpToCurTime();
	rowStyle = frmSettings.rowStyle();
	if ( dayView )
	    dayView->setStartViewTime( startTime );
	    dayView->setJumpToCurTime( bJumpToCurTime );
	    dayView->setRowStyle( rowStyle );
	if ( weekView )
	    weekView->setStartViewTime( startTime );
	saveSettings();

	// make the change obvious
	if ( views->visibleWidget() ) {
	    if ( views->visibleWidget() == dayView )
		dayView->redraw();
	    else if ( views->visibleWidget() == weekView )
		weekView->redraw();
	}
    }
}

void DateBook::fileNew()
{
    slotNewEventFromKey("");
}

QString DateBook::checkEvent(const Event &e)
{
    /* check if overlaps with itself */
    bool checkFailed = FALSE;

    /* check the next 12 repeats. should catch most problems */
    QDate current_date = e.start().date();
    Event previous = e;
    for(int i = 0; i < 12; i++)
    {
	QDateTime next;
	if (!nextOccurance(previous, current_date.addDays(1), next)) {
	    break;  // no more repeats
	}
	if(next < previous.end()) {
	    checkFailed = TRUE;
	    break;
	}
	current_date = next.date();
    }

    if(checkFailed)
	return tr("Event duration is potentially longer\n"
		  "than interval between repeats.");

    return QString::null;
}

QDate DateBook::currentDate()
{
    QDate d = QDate::currentDate();

    if ( dayView && views->visibleWidget() == dayView ) {
	d = dayView->date();
    } else if ( weekView && views->visibleWidget() == weekView ) {
        d = weekView->date();
    } else if ( weekLstView && views->visibleWidget() == weekLstView ) {
        d = weekLstView->date();
    } else if ( monthView && views->visibleWidget() == monthView ) {
	d = monthView->selectedDate();
    }

    return d;
}

void DateBook::view(int v, const QDate &d) {
    if (v==DAY) {
	initDay();
	dayAction->setOn( TRUE );
	dayView->setDate( d );
	views->raiseWidget( dayView );
	dayView->redraw();
    } else if (v==WEEK) {
	initWeek();
	weekAction->setOn( TRUE );
	weekView->setDate( d );
	views->raiseWidget( weekView );
	weekView->redraw();
    } else if (v==WEEKLST) {
	initWeekLst();
	weekLstAction->setOn( TRUE );
	weekLstView->setDate(d);
	views->raiseWidget( weekLstView );
	weekLstView->redraw();
    } else if (v==MONTH) {
	initMonth();
	monthAction->setOn( TRUE );
	monthView->setDate( d.year(), d.month(), d.day() );
	views->raiseWidget( monthView );
	monthView->redraw();
    }
}

void DateBook::viewDefault(const QDate &d) {
  Config config("DateBook");
  config.setGroup("Main");
  int current=config.readNumEntry("defaultview", DAY);

  view(current,d);
}

void DateBook::viewDay() {
  view(DAY,currentDate());
}

void DateBook::viewWeek() {
    view(WEEK,currentDate());
}

void DateBook::viewWeekLst() {
    view(WEEKLST,currentDate());
}

void DateBook::viewMonth() {
    view(MONTH,currentDate());
}

void DateBook::editEvent( const Event &e )
{
    if (syncing) {
	QMessageBox::warning( this, tr("Calendar"),
	                      tr( "Can not edit data, currently syncing") );
	return;
    }

    // workaround added for text input.
    QDialog editDlg( this, 0, TRUE );
    DateEntry *entry;
    editDlg.setCaption( tr("Edit Event") );
    QVBoxLayout *vb = new QVBoxLayout( &editDlg );
    QScrollView *sv = new QScrollView( &editDlg, "scrollview" );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    // KLUDGE!!!
    sv->setHScrollBarMode( QScrollView::AlwaysOff );
    vb->addWidget( sv );
    entry = new DateEntry( onMonday, e, ampm, &editDlg, "editor" );
    entry->timezone->setEnabled( FALSE );
    sv->addChild( entry );

#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    editDlg.showMaximized();
#endif
    while (editDlg.exec() ) {
	Event newEv = entry->event();
	if(newEv.description().isEmpty() && newEv.notes().isEmpty() )
	    break;
	newEv.setUid(e.uid()); // FIXME: Hack not to clear uid
	QString error = checkEvent(newEv);
	if (!error.isNull()) {
	    if (QMessageBox::warning(this, "error box",
				     error, "Fix it", "Continue",
				     0, 0, 1) == 0)
		continue;
	}
	db->editEvent(e, newEv);
	emit newEvent();
	break;
    }
}

void DateBook::removeEvent( const Event &e )
{
    if (syncing) {
	QMessageBox::warning( this, tr("Calendar"),
	                      tr( "Can not edit data, currently syncing") );
	return;
    }

    QString strName = e.description();

    if ( !QPEMessageBox::confirmDelete( this, tr( "Calendar" ),strName ) )
	return;

    db->removeEvent( e );
    if ( views->visibleWidget() == dayView && dayView )
        dayView->redraw();
}

void DateBook::addEvent( const Event &e )
{
    QDate d = e.start().date();
    initDay();
    dayView->setDate( d );
}

void DateBook::showDay( int year, int month, int day )
{
    QDate d(year, month, day);
    view(DAY,d);
}

void DateBook::initDay()
{
    if ( !dayView ) {
	dayView = new DateBookDay( ampm, onMonday, db, views, "day view" );
	views->addWidget( dayView, DAY );
	dayView->setStartViewTime( startTime );
	dayView->setJumpToCurTime( bJumpToCurTime );
	dayView->setRowStyle( rowStyle );
	connect( this, SIGNAL( newEvent() ),
		 dayView, SLOT( redraw() ) );
	connect( dayView, SIGNAL( newEvent() ),
		 this, SLOT( fileNew() ) );
	connect( dayView, SIGNAL( removeEvent( const Event & ) ),
		 this, SLOT( removeEvent( const Event & ) ) );
	connect( dayView, SIGNAL( editEvent( const Event & ) ),
		 this, SLOT( editEvent( const Event & ) ) );
	connect( dayView, SIGNAL( beamEvent( const Event & ) ),
		 this, SLOT( beamEvent( const Event & ) ) );
	connect( dayView, SIGNAL(sigNewEvent(const QString &)),
		 this, SLOT(slotNewEventFromKey(const QString &)) );
    }
}

void DateBook::initWeek()
{
    if ( !weekView ) {
	weekView = new DateBookWeek( ampm, onMonday, db, views, "week view" );
	weekView->setStartViewTime( startTime );
	views->addWidget( weekView, WEEK );
	connect( weekView, SIGNAL( showDate( int, int, int ) ),
		 this, SLOT( showDay( int, int, int ) ) );
	connect( this, SIGNAL( newEvent() ),
		 weekView, SLOT( redraw() ) );
    }
    //But also get it right: the year that we display can be different
    //from the year of the current date. So, first find the year
    //number of the current week.

    int yearNumber, totWeeks;
    calcWeek( currentDate(), totWeeks, yearNumber, onMonday );

    QDate d = QDate( yearNumber, 12, 31 );
    calcWeek( d, totWeeks, yearNumber, onMonday );

    while ( totWeeks == 1 ) {
	d = d.addDays( -1 );
	calcWeek( d, totWeeks, yearNumber, onMonday );
    }
    if ( totWeeks != weekView->totalWeeks() )
	weekView->setTotalWeeks( totWeeks );
}
void DateBook::initWeekLst() {
  if ( !weekLstView ) {
      weekLstView = new DateBookWeekLst( ampm, onMonday, db,
					 views, "weeklst view" );
      views->addWidget( weekLstView, WEEKLST );

      //weekLstView->setStartViewTime( startTime );
      connect( weekLstView, SIGNAL( showDate( int, int, int ) ),
	       this, SLOT( showDay( int, int, int ) ) );
      connect( weekLstView, SIGNAL( addEvent( const QDateTime &,
					      const QDateTime &,
					      const QString & ) ),
	       this, SLOT( slotNewEntry( const QDateTime &,
					 const QDateTime &,
					 const QString & ) ) );
      connect( this, SIGNAL( newEvent() ),
	       weekLstView, SLOT( redraw() ) );
      connect( weekLstView, SIGNAL( editEvent( const Event & ) ),
	       this, SLOT( editEvent( const Event & ) ) );
  }
}


void DateBook::initMonth()
{
    if ( !monthView ) {
	monthView = new DateBookMonth( views, "month view", FALSE, db );
	views->addWidget( monthView, MONTH );
	connect( monthView, SIGNAL( dateClicked( int, int, int ) ),
             this, SLOT( showDay( int, int, int ) ) );
	connect( this, SIGNAL( newEvent() ),
		 monthView, SLOT( redraw() ) );
	qApp->processEvents();
    }
}

void DateBook::loadSettings()
{
    {
	Config config( "qpe" );
	config.setGroup("Time");
	ampm = config.readBoolEntry( "AMPM", TRUE );
	onMonday = config.readBoolEntry( "MONDAY" );
    }

    {
	Config config("DateBook");
	config.setGroup("Main");
	startTime = config.readNumEntry("startviewtime", 8);
	aPreset = config.readBoolEntry("alarmpreset");
	presetTime = config.readNumEntry("presettime");
	bJumpToCurTime = config.readBoolEntry("jumptocurtime");
	rowStyle = config.readNumEntry("rowstyle");
    }
}

void DateBook::saveSettings()
{
    Config config( "qpe" );
    Config configDB( "DateBook" );
    configDB.setGroup( "Main" );
    configDB.writeEntry("startviewtime",startTime);
    configDB.writeEntry("alarmpreset",aPreset);
    configDB.writeEntry("presettime",presetTime);
    configDB.writeEntry("jumptocurtime", bJumpToCurTime);
    configDB.writeEntry("rowstyle", rowStyle);
}

void DateBook::newDefaultView(QAction *a) {
    int val=DAY;
    if (a->text() == "Day") val=DAY;
    if (a->text() == "Week") val=WEEK;
    if (a->text() == "WeekLst") val=WEEKLST;
    if (a->text() == "Month") val=MONTH;

    Config configDB( "DateBook" );
    configDB.setGroup( "Main" );
    configDB.writeEntry("defaultview",val);
}

void DateBook::appMessage(const QCString& msg, const QByteArray& data)
{
    bool needShow = FALSE;
    if ( msg == "alarm(QDateTime,int)" ) {
	QDataStream ds(data,IO_ReadOnly);
	QDateTime when; int warn;
	ds >> when >> warn;

	// check to make it's okay to continue,
	// this is the case that the time was set ahead, and
	// we are forced given a stale alarm...
	QDateTime current = QDateTime::currentDateTime();
	if ( current.time().hour() != when.time().hour()
	     && current.time().minute() != when.time().minute() )
	    return;

	QValueList<EffectiveEvent> list = db->getEffectiveEvents(when.addSecs(warn*60));
	if ( list.count() > 0 ) {
	    QString msg;
	    bool bSound = FALSE;
	    int stopTimer = 0;
	    bool found = FALSE;
	    for ( QValueList<EffectiveEvent>::ConstIterator it=list.begin();
		  it!=list.end(); ++it ) {
		if ( (*it).event().hasAlarm() ) {
		    found = TRUE;
		    msg += "<CENTER><B>" + (*it).description() + "</B>"
			   + "<BR>" + (*it).location() + "<BR>"
			   + TimeString::dateString((*it).event().start(),ampm)
			   + (warn
			      ? tr(" (in " + QString::number(warn)
				   + tr(" minutes)"))
			      : QString(""))
			   + "<BR>"
			   + (*it).notes() + "</CENTER>";
		    if ( (*it).event().alarmSound() != Event::Silent ) {
			bSound = TRUE;
		    }
		}
	    }
	    if ( found ) {
		if ( bSound ) {
		    Sound::soundAlarm();
		    alarmCounter = 0;
		    stopTimer = startTimer( 5000 );
		}

		QDialog dlg( this, 0, TRUE );
		QVBoxLayout *vb = new QVBoxLayout( &dlg );
		QScrollView *view = new QScrollView( &dlg, "scrollView");
		view->setResizePolicy( QScrollView::AutoOneFit );
		vb->addWidget( view );
		QLabel *lblMsg = new QLabel( msg, &dlg );
		view->addChild( lblMsg );
		QPushButton *cmdOk = new QPushButton( tr("OK"), &dlg );
		connect( cmdOk, SIGNAL(clicked()), &dlg, SLOT(accept()) );
		vb->addWidget( cmdOk );

#if defined(Q_WS_QWS) || defined(_WS_QWS_)
		dlg.showMaximized();
#endif
		needShow = dlg.exec();

		if ( bSound )
		    killTimer( stopTimer );
	    }
	}
    } else if ( msg == "nextView()" ) {
	QWidget* cur = views->visibleWidget();
	if ( cur ) {
	    if ( cur == dayView )
		viewWeek();
	    else if ( cur == weekView )
		viewWeekLst();
	    else if ( cur == weekLstView )
		viewMonth();
	    else if ( cur == monthView )
		viewDay();
	    needShow = TRUE;
	}
    }
    if ( needShow ) {
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
	showMaximized();
#else
	show();
#endif
	raise();
	QPEApplication::setKeepRunning();
	setActiveWindow();
    }
}

void DateBook::reload()
{
    db->reload();
    if ( dayAction->isOn() )
	viewDay();
    else if ( weekAction->isOn() )
	viewWeek();
    else if ( monthAction->isOn() )
	viewMonth();
    syncing = FALSE;
}

void DateBook::flush()
{
    syncing = TRUE;
    db->save();
}

void DateBook::timerEvent( QTimerEvent *e )
{
    if ( alarmCounter < 10 ) {
	alarmCounter++;
	Sound::soundAlarm();
    }
    else
	killTimer( e->timerId() );
}

void DateBook::changeClock( bool newClock )
{
    ampm = newClock;
    // repaint the affected objects...
    if (dayView) dayView->redraw();
    if (weekView) weekView->redraw();
    if (weekLstView) weekLstView->redraw();
}

void DateBook::changeWeek( bool m )
{
    /* no need to redraw, each widget catches.  Do need to
       store though for widgets we haven't made yet */
    onMonday = m;
}

void DateBook::slotToday()
{
    // we need to view today using default view
    viewDefault(QDate::currentDate());
}

void DateBook::closeEvent( QCloseEvent *e )
{
    if(syncing) {
	/* no need to save, did that at flush */
	e->accept();
	return;
    }

    // save settings will generate it's own error messages, no
    // need to do checking ourselves.
    saveSettings();
    if ( db->save() )
	e->accept();
    else {
	if ( QMessageBox::critical( this, tr( "Out of space" ),
				    tr("Calendar was unable to save\n"
				       "your changes.\n"
				       "Free up some space and try again.\n"
				       "\nQuit anyway?"),
				    QMessageBox::Yes|QMessageBox::Escape,
				    QMessageBox::No|QMessageBox::Default )
	     != QMessageBox::No )
	    e->accept();
	else
	    e->ignore();
    }
}

// Entering directly from the "keyboard"
void DateBook::slotNewEventFromKey( const QString &str )
{
    if (syncing) {
	QMessageBox::warning( this, tr("Calendar"),
	                      tr( "Can not edit data, currently syncing") );
	return;
    }

    // We get to here from a key pressed in the Day View
    // So we can assume some things.  We want the string
    // passed in to be part of the description.
    QDateTime start, end;
    if ( views->visibleWidget() == dayView ) {
	dayView->selectedDates( start, end );
    } else if ( views->visibleWidget() == monthView ) {
	QDate d = monthView->selectedDate();
	start = end = d;
	start.setTime( QTime( 10, 0 ) );
	end.setTime( QTime( 12, 0 ) );
    } else if ( views->visibleWidget() == weekView ) {
	QDate d = weekView->date();
	start = end = d;
	start.setTime( QTime( 10, 0 ) );
	end.setTime( QTime( 12, 0 ) );
    }
    slotNewEntry(start, end, str);
}
void DateBook::slotNewEntry(const QDateTime &start, const QDateTime &end, const QString &str) {
    // argh!  This really needs to be encapsulated in a class
    // or function.
    QDialog newDlg( this, 0, TRUE );
    newDlg.setCaption( DateEntryBase::tr("New Event") );
    DateEntry *e;
    QVBoxLayout *vb = new QVBoxLayout( &newDlg );
    QScrollView *sv = new QScrollView( &newDlg );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setFrameStyle( QFrame::NoFrame );
    sv->setHScrollBarMode( QScrollView::AlwaysOff );
    vb->addWidget( sv );

    Event ev;
    ev.setDescription(  str );
    // When the new gui comes in, change this...
    ev.setLocation( tr("(Unknown)") );
    ev.setStart( start );
    ev.setEnd( end );

    e = new DateEntry( onMonday, ev, ampm, &newDlg );
    e->setAlarmEnabled( aPreset, presetTime, Event::Loud );
    sv->addChild( e );
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    newDlg.showMaximized();
#endif
    while (newDlg.exec()) {
	ev = e->event();
	ev.assignUid();
	QString error = checkEvent( ev );
	if ( !error.isNull() ) {
	    if ( QMessageBox::warning( this, tr("Error!"),
				       error, tr("Fix it"), tr("Continue"), 0, 0, 1 ) == 0 )
		continue;
	}
	db->addEvent( ev );
	emit newEvent();
	break;
    }
}

void DateBook::setDocument( const QString &filename )
{
    if ( filename.find(".vcs") != int(filename.length()) - 4 ) return;

    QValueList<Event> tl = Event::readVCalendar( filename );
    for( QValueList<Event>::Iterator it = tl.begin(); it != tl.end(); ++it ) {
	db->addEvent( *it );
    }
}

static const char * beamfile = "/tmp/obex/event.vcs";

void DateBook::beamEvent( const Event &e )
{
    qDebug("trying to beamn");
    unlink( beamfile ); // delete if exists
    mkdir("/tmp/obex/", 0755);
    Event::writeVCalendar( beamfile, e );
    Ir *ir = new Ir( this );
    connect( ir, SIGNAL( done( Ir * ) ), this, SLOT( beamDone( Ir * ) ) );
    QString description = e.description();
    ir->send( beamfile, description, "text/x-vCalendar" );
}

void DateBook::beamDone( Ir *ir )
{
    delete ir;
    unlink( beamfile );
}

void DateBook::slotFind()
{
    // move it to the day view...
    viewDay();
    FindDialog frmFind( "Calendar", this );
    frmFind.setUseDate( true );
    frmFind.setDate( currentDate() );
    QObject::connect( &frmFind,
                      SIGNAL(signalFindClicked(const QString&, const QDate&,
					       bool, bool, int)),
		      this,
		      SLOT(slotDoFind(const QString&, const QDate&,
				      bool, bool, int)) );
    QObject::connect( this,
		      SIGNAL(signalNotFound()),
		      &frmFind,
		      SLOT(slotNotFound()) );
    QObject::connect( this,
		      SIGNAL(signalWrapAround()),
		      &frmFind,
		      SLOT(slotWrapAround()) );
    frmFind.exec();
    inSearch = false;
}

bool catComp( QArray<int> cats, int category )
{
    bool returnMe;
    int i,
	count;

    count = int(cats.count());
    returnMe = false;
    if ( (category == -1 && count == 0) || category == -2 )
	returnMe = true;
    else {
	for ( i = 0; i < count; i++ ) {
	    if ( category == cats[i] ) {
		returnMe = true;
		break;
	    }
	}
    }
    return returnMe;
}


void DateBook::slotDoFind( const QString& txt, const QDate &dt,
			   bool caseSensitive, bool /*backwards*/,
			   int category )
{
    QDateTime dtEnd( QDate(3001, 1, 1), QTime(0, 0, 0) ),
	next;

    QRegExp r( txt );
    r.setCaseSensitive( caseSensitive );


    static Event rev,
	nonrev;
    if ( !inSearch ) {
	rev.setStart( QDateTime(QDate(1960, 1, 1), QTime(0, 0, 0)) );
	nonrev.setStart( rev.start() );
	inSearch = true;
    }
    static QDate searchDate = dt;
    static bool wrapAround = true;
    bool candidtate;
    candidtate = false;

    QValueList<Event> repeats = db->getRawRepeats();

    // find the candidate for the first repeat that matches...
    QValueListConstIterator<Event> it;
    QDate start = dt;
    for ( it = repeats.begin(); it != repeats.end(); ++it ) {
	if ( catComp( (*it).categories(), category ) ) {
	    while ( nextOccurance( *it, start, next ) ) {
		if ( next < dtEnd ) {
		    if ( (*it).match( r ) && !(next <= rev.start()) ) {
			rev = *it;
			dtEnd = next;
			rev.setStart( next );
			candidtate = true;
			wrapAround = true;
			start = dt;
			break;
		    } else
			start = next.date().addDays( 1 );
		}
	    }
	}
    }

    // now the for first non repeat...
    QValueList<Event> nonRepeats = db->getNonRepeatingEvents( dt, dtEnd.date() );
    qHeapSort( nonRepeats.begin(), nonRepeats.end() );
    for ( it = nonRepeats.begin(); it != nonRepeats.end(); ++it ) {
	if ( catComp( (*it).categories(), category ) ) {
	    if ( (*it).start() < dtEnd ) {
		if ( (*it).match( r ) && !(*it <= nonrev) ) {
		    nonrev = *it;
		    dtEnd = nonrev.start();
		    candidtate = true;
		    wrapAround = true;
		    break;
		}
	    }
	}
    }
    if ( candidtate ) {
	dayView->setStartViewTime( dtEnd.time().hour() );
	dayView->setDate( dtEnd.date().year(), dtEnd.date().month(),
			  dtEnd.date().day() );
    } else {
	if ( wrapAround ) {
	    emit signalWrapAround();
	    rev.setStart( QDateTime(QDate(1960, 1, 1), QTime(0, 0, 0)) );
	    nonrev.setStart( rev.start() );
	} else
	    emit signalNotFound();
	wrapAround = !wrapAround;
    }
}

Event DateBookDBHack::eventByUID(int uid) {

    // FIXME: Dirty Hacks to get hold of the private event lists
    QDate start;
    QDate end=start.addDays(-1);
    QValueList<Event> myEventList=getNonRepeatingEvents(start,end);
    QValueList<Event> myRepeatEvents=getRawRepeats();

    QValueList<Event>::ConstIterator it;

    for (it = myEventList.begin(); it != myEventList.end(); it++) {
	if ((*it).uid() == uid) return *it;
    }
    for (it = myRepeatEvents.begin(); it != myRepeatEvents.end(); it++) {
	if ((*it).uid() == uid) return *it;
    }

    qDebug("Event not found: uid=%d\n", uid);
    Event ev;
    return ev; // return at least
}

