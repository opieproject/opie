/*
 * today.cpp : main class
 *
 * ---------------------
 *
 * begin       : Sun 10 17:20:00 CEST 2002
 * copyright   : (c) 2002 by Maximilian Reiﬂ
 * email       : max.reiss@gmx.de
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "today.h"

#include <qpe/timestring.h>
#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
//#include <qpe/qprocess.h>
#include <qpe/resource.h>
#include <qpe/contact.h>
#include <qpe/global.h>
#include <qpe/qpeapplication.h>

#include <qdir.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qtextstream.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qpixmap.h>
//#include <qfileinfo.h>
#include <qlayout.h>
#include <qtl.h>


//#include <iostream.h>
#include <unistd.h>
#include <stdlib.h>

int MAX_LINES_TASK;
int MAX_CHAR_CLIP;
int MAX_LINES_MEET;
int SHOW_LOCATION;
int SHOW_NOTES;
// show only later dates
int ONLY_LATER;
int AUTOSTART;
int NEW_START=1;
QString AUTOSTART_TIMER;

/*
 *  Constructs a Example which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
Today::Today( QWidget* parent,  const char* name, WFlags fl )
  : TodayBase( parent, name, fl ), AllDateBookEvents(NULL) {
  QObject::connect( (QObject*)PushButton1, SIGNAL( clicked() ), this, SLOT(startConfig() ) );
  QObject::connect( (QObject*)TodoButton, SIGNAL( clicked() ), this, SLOT(startTodo() ) );
  QObject::connect( (QObject*)DatesButton, SIGNAL( clicked() ), this, SLOT(startDatebook() ) );
  QObject::connect( (QObject*)MailButton, SIGNAL( clicked() ), this, SLOT(startMail() ) );

#if defined(Q_WS_QWS)
#if !defined(QT_NO_COP)
  QCopChannel *todayChannel = new QCopChannel("QPE/Today" , this );
  connect (todayChannel, SIGNAL( received(const QCString &, const QByteArray &)),
	   this, SLOT ( channelReceived(const QCString &, const QByteArray &)) );
#endif
#endif



  db = NULL;
  setOwnerField();
  todo = new ToDoDB;
  getTodo();
  draw();
  autoStart();
}

/*
 * Qcop receive method.
 */
void Today::channelReceived(const QCString &msg, const QByteArray & data) {
  QDataStream stream(data, IO_ReadOnly );
  if ( msg == "message(QString)" ) {
    QString message;
    stream >> message;
    setOwnerField(message);
  }

}

/*
 * Initialises the owner field with the default value, the username
 */
void Today::setOwnerField() {
  QString file = Global::applicationFileName("addressbook", "businesscard.vcf");
  if (QFile::exists(file)) {
    Contact cont = Contact::readVCard(file)[0];
    QString returnString = cont.fullName();
    OwnerField->setText( "<b>" +tr ("Owned by ") + returnString + "</b>");
  } else {
    OwnerField->setText( "<b>" + tr ("Please fill out the business card")+" </b>");
  }
}

/*
 * Set the owner field with a given QString, for example per qcop.
 */
void Today::setOwnerField(QString &message) {
  if (!message.isEmpty()) {
    OwnerField->setText("<b>" + message + "</b>");
  }
}

/*
 * Autostart, uses the new (opie only) autostart method in the launcher code.
 * If registered against that today ist started on each resume.
 */
void Today::autoStart() {
    Config cfg("today");
    cfg.setGroup("Autostart");
    int AUTOSTART = cfg.readNumEntry("autostart",1);
//    qDebug(QString("%1").arg(AUTOSTART));
    if (AUTOSTART) {
        QCopEnvelope e("QPE/System", "autoStart(QString, QString, QString)");
        e << QString("add");
        e << QString("today");
        e << AUTOSTART_TIMER;
    } else {
        qDebug("Nun in else bei autostart");
        QCopEnvelope e("QPE/System", "autoStart(QString, QString)");
        e << QString("remove");
        e << QString("today");
    }
}

/*
 * Repaint method. Reread all fields.
 */
void Today::draw() {
  init();
  getDates();
  getMail();

  // if the todolist.xml file was not modified in between, do not parse it.
  if (checkIfModified()) {
    if (todo) delete todo;
    todo = new ToDoDB;
    getTodo();
  }

  // how often refresh
  QTimer::singleShot( 20*1000, this, SLOT(draw() ) );
}

/*
 * Check if the todolist.xml was modified (if there are new entries.
 * Returns true if it was modified.
 */
bool Today::checkIfModified() {

  QDir dir;
  QString homedir = dir.homeDirPath ();
  QString time;

  Config cfg("today");
  cfg.setGroup("Files");
  time = cfg.readEntry("todolisttimestamp", "");

  QFileInfo file = (homedir +"/Applications/todolist/todolist.xml");
  QDateTime fileTime  = file.lastModified();
  if (time.compare(fileTime.toString()) == 0) {
    return false;
  } else {
    cfg.writeEntry("todolisttimestamp", fileTime.toString() );
    cfg.write();
    return true;
  }
}


/*
 * Init stuff needed for today. Reads the config file.
 */
void Today::init() {
  QDate date = QDate::currentDate();
  QString time = (tr( date.toString()) );

  TextLabel1->setText(QString("<font color=#FFFFFF>" + time + "</font>"));

  // read config
  Config cfg("today");
  cfg.setGroup("BaseConfig");

  // -- config file section --
  // how many lines should be showed in the task section
  MAX_LINES_TASK = cfg.readNumEntry("maxlinestask",5);
  // after how many chars should the be cut off on tasks and notes
  MAX_CHAR_CLIP = cfg.readNumEntry("maxcharclip",40);
  // how many lines should be showed in the datebook section
  MAX_LINES_MEET = cfg.readNumEntry("maxlinesmeet",5);
  // If location is to be showed too, 1 to activate it.
  SHOW_LOCATION = cfg.readNumEntry("showlocation",1);
  // if notes should be shown
  SHOW_NOTES = cfg.readNumEntry("shownotes",0);
  // should only later appointments be shown or all for the current day.
  ONLY_LATER = cfg.readNumEntry("onlylater",1);

  cfg.setGroup("Autostart");
  AUTOSTART = cfg.readNumEntry("autostart",1);
  AUTOSTART_TIMER = cfg.readEntry("autostartdelay", "0");

  //db = new DateBookDB;
}

/*
 * The method for the configuration dialog.
 */
void Today::startConfig() {

  conf = new todayconfig ( this, "", true );
  // read the config
  Config cfg("today");
  cfg.setGroup("BaseConfig");

  //init();

  conf->SpinBox1->setValue(MAX_LINES_MEET);
  // location show box
  conf->CheckBox1->setChecked(SHOW_LOCATION);
  // notes show box
  conf->CheckBox2->setChecked(SHOW_NOTES);
  // task lines
  conf->SpinBox2->setValue(MAX_LINES_TASK);
  // clip when?
  conf->SpinBox7->setValue(MAX_CHAR_CLIP);
  // only later
  conf->CheckBox3->setChecked(ONLY_LATER);
  // if today should be autostarted
  conf->CheckBoxAuto->setChecked(AUTOSTART);
  // autostart only if device has been suspended for X minutes
  conf->SpinBoxTime->setValue( AUTOSTART_TIMER.toInt() );

  conf->exec();

  int maxlinestask = conf->SpinBox2->value();
  int maxmeet = conf->SpinBox1->value();
  int location = conf->CheckBox1->isChecked();
  int notes = conf->CheckBox2->isChecked();
  int maxcharclip = conf->SpinBox7->value();
  int onlylater = conf->CheckBox3->isChecked();
  int autostart = conf->CheckBoxAuto->isChecked();
  int autostartdelay = conf->SpinBoxTime->value();


  cfg.writeEntry("maxlinestask",maxlinestask);
  cfg.writeEntry("maxcharclip", maxcharclip);
  cfg.writeEntry("maxlinesmeet",maxmeet);
  cfg.writeEntry("showlocation",location);
  cfg.writeEntry("shownotes", notes);
  cfg.writeEntry("onlylater", onlylater);
  cfg.setGroup("Autostart");
  cfg.writeEntry("autostart", autostart);
  cfg.writeEntry("autostartdelay", autostartdelay);

  // sync it to "disk"
  cfg.write();
  NEW_START=1;
  draw();
  AUTOSTART=autostart;
  autoStart();
}


/*
 *  Get all events that are in the datebook xml file for today
 */
void Today::getDates() {
  QDate date = QDate::currentDate();

  if (AllDateBookEvents) delete AllDateBookEvents;
  AllDateBookEvents = new QWidget( );
  QVBoxLayout* layoutDates = new QVBoxLayout(AllDateBookEvents);

  if (db) {
   delete db;
  }
  db = new DateBookDB;

  QValueList<EffectiveEvent> list = db->getEffectiveEvents(date, date);

  qBubbleSort(list);
  // printf("Get dates\n");

  Config config( "qpe" );
  // if 24 h format
  //bool ampm = config.readBoolEntry( "AMPM", TRUE );

  int count=0;

  if ( list.count() > 0 ) {

    for ( QValueList<EffectiveEvent>::ConstIterator it=list.begin();
    it!=list.end(); ++it ) {


      if ( count <= MAX_LINES_MEET ) {

	QTime time = QTime::currentTime();

	if (!ONLY_LATER) {
	  count++;
	  DateBookEvent *l=new DateBookEvent(*it, AllDateBookEvents, SHOW_LOCATION, SHOW_NOTES);
	  layoutDates->addWidget(l);
	  connect (l, SIGNAL(editEvent(const Event &)),
		   this, SLOT(editEvent(const Event &)));
	} else if ((time.toString() <= TimeString::dateString((*it).event().end())) ) {
	  count++;

	  // show only later appointments
	  DateBookEventLater *l=new DateBookEventLater(*it, AllDateBookEvents, SHOW_LOCATION, SHOW_NOTES);
	  layoutDates->addWidget(l);
	  connect (l, SIGNAL(editEvent(const Event &)),
		   this, SLOT(editEvent(const Event &)));
	}
      }
    }
    if (ONLY_LATER && count==0) {
	QLabel* noMoreEvents = new QLabel(AllDateBookEvents);
	noMoreEvents->setText(tr("No more appointments today"));
	layoutDates->addWidget(noMoreEvents);
    }
  } else {
    QLabel* noEvents = new QLabel(AllDateBookEvents);
    noEvents->setText(tr("No appointments today"));
    layoutDates->addWidget(noEvents);
  }

  layoutDates->addItem(new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::Expanding));
  sv1->addChild(AllDateBookEvents);
  AllDateBookEvents->show();
}


void Today::getMail() {
  Config cfg("opiemail");
  cfg.setGroup("today");

  // how many lines should be showed in the task section
  int NEW_MAILS = cfg.readNumEntry("newmails",0);
  int OUTGOING = cfg.readNumEntry("outgoing",0);

  QString output = tr("<b>%1</b> new mail(s), <b>%2</b> outgoing").arg(NEW_MAILS).arg(OUTGOING);

  MailField->setText(output);
}


/*
 * Get the todos
 */
void Today::getTodo() {

  QString output;
  QString tmpout;
  int count = 0;
  int ammount = 0;

  // get overdue todos first
  QValueList<ToDoEvent> overDueList = todo->overDue();
  qBubbleSort(overDueList);
  for ( QValueList<ToDoEvent>::Iterator it=overDueList.begin();
	it!=overDueList.end(); ++it ) {
    if (!(*it).isCompleted() && ( ammount < MAX_LINES_TASK) ) {
      tmpout += "<font color=#e00000><b>-" +((*it).description()).mid(0, MAX_CHAR_CLIP) + "</b></font><br>";
      ammount++;
    }
  }

  // get total number of still open todos
  QValueList<ToDoEvent> open = todo->rawToDos();
  qBubbleSort(open);
  for ( QValueList<ToDoEvent>::Iterator it=open.begin();
	it!=open.end(); ++it ) {
    if (!(*it).isCompleted()){
      count +=1;
      // not the overdues, we allready got them, and not if we are
      // over the maxlines
      if (!(*it).isOverdue() && ( ammount < MAX_LINES_TASK) ) {
	tmpout += "<b>-</b>" + ((*it).description()).mid(0, MAX_CHAR_CLIP) + "<br>";
	ammount++;
      }
    }
  }


  if (count > 0) {
    if( count == 1 ) {
      output = tr("There is <b> 1</b> active task:  <br>" );
    } else {
      output = tr("There are <b> %1</b> active tasks: <br>").arg(count);
    }
    output += tmpout;
  } else {
    output = tr("No active tasks");
  }

  TodoField->setText(tr(output));
}

/*
 * launches datebook
 */
void Today::startDatebook() {
  QCopEnvelope e("QPE/System", "execute(QString)");
  e << QString("datebook");
}

/*
 * starts the edit dialog as known from datebook
 */

extern QPEApplication *todayApp;

void Today::editEvent(const Event &e) {
  startDatebook();

  while(!QCopChannel::isRegistered("QPE/Datebook")) todayApp->processEvents();
  QCopEnvelope env("QPE/Datebook", "editEvent(int)");
  env << e.uid();
}

/*
 * launches todolist
 */
void Today::startTodo() {
  QCopEnvelope e("QPE/System", "execute(QString)");
  e << QString("todolist");
}

/*
 * launch opiemail
 */
void Today::startMail() {
  QCopEnvelope e("QPE/System", "execute(QString)");
  e << QString("opiemail");
//Right now start both, maybe decide which to rum via config file ..
  QCopEnvelope f("QPE/System", "execute(QString)");
  f << QString("qtmail");
}


Today::~Today() {
}

/*
 * Gets the events for the current day, if it should get all dates
 */
DateBookEvent::DateBookEvent(const EffectiveEvent &ev,
			     QWidget* parent,
			     int SHOW_LOCATION,
			     int SHOW_NOTES,
			     const char* name,
			     WFlags fl) :
  OClickableLabel(parent,name,fl), event(ev) {

  QString msg;
  //QTime time = QTime::currentTime();

   Config config( "qpe" );
   config.setGroup( "Time" );
  // if 24 h format
   ampm = config.readBoolEntry( "AMPM", TRUE );


  if (!ONLY_LATER) {
    msg += "<B>" + (ev).description() + "</B>";
    if ( (ev).event().hasAlarm() ) {
      msg += " <b>[with alarm]</b>";
    }
    // include location or not
    if (SHOW_LOCATION == 1) {
      msg += "<BR><i>" + (ev).location() + "</i>";
    }

    if ( (TimeString::timeString(QTime((ev).event().start().time()) ) == "00:00") &&  (TimeString::timeString(QTime((ev).event().end().time()) ) == "23:59") ) {
      msg += "<br>All day";
    }  else {
      // start time of event
        msg += "<br>" + ampmTime(QTime((ev).event().start().time()) )
        // end time of event
        + "<b> - </b>" + ampmTime(QTime((ev).event().end().time()) );
    }

    // include possible note or not
    if (SHOW_NOTES == 1) {
      msg += "<br> <i>note</i>:" +((ev).notes()).mid(0, MAX_CHAR_CLIP);
    }
  }
  setText(msg);
  connect(this, SIGNAL(clicked()), this, SLOT(editMe()));
  setAlignment( int( QLabel::WordBreak | QLabel::AlignLeft ) );
}


QString DateBookEvent::ampmTime(QTime tm) {

    QString s;
    if( ampm ) {
	int hour = tm.hour();
	if (hour == 0)
	    hour = 12;
	if (hour > 12)
	    hour -= 12;
	s.sprintf( "%2d:%02d %s", hour, tm.minute(),
		   (tm.hour() >= 12) ? "PM" : "AM" );
        return s;
    } else {
	s.sprintf( "%2d:%02d", tm.hour(), tm.minute() );
        return s;
    }

}


DateBookEventLater::DateBookEventLater(const EffectiveEvent &ev,
				       QWidget* parent,
				       int SHOW_LOCATION,
				       int SHOW_NOTES,
				       const char* name,
				       WFlags fl) :
  OClickableLabel(parent,name,fl), event(ev) {

  QString msg;
  QTime time = QTime::currentTime();

  Config config( "qpe" );
  config.setGroup( "Time" );
  // if 24 h format
  ampm = config.readBoolEntry( "AMPM", TRUE );


  if ((time.toString() <= TimeString::dateString((ev).event().end())) ) {
    // show only later appointments
    msg += "<B>" + (ev).description() + "</B>";
    if ( (ev).event().hasAlarm() ) {
      msg += " <b>[with alarm]</b>";
    }
    // include location or not
    if (SHOW_LOCATION == 1) {
      msg += "<BR><i>" + (ev).location() + "</i>";
    }

    if ( (TimeString::timeString(QTime((ev).event().start().time()) ) == "00:00") &&  (TimeString::timeString(QTime((ev).event().end().time()) ) == "23:59") ) {
      msg += "<br>All day";
    } else {
      // start time of event
        msg += "<br>" + ampmTime(QTime((ev).event().start().time()) )
        // end time of event
        + "<b> - </b>" + ampmTime(QTime((ev).event().end().time()) );
    }
    // include possible note or not
    if (SHOW_NOTES == 1) {
      msg += "<br> <i>note</i>:" +((ev).notes()).mid(0, MAX_CHAR_CLIP);
    }
  }

  setText(msg);
  connect(this, SIGNAL(clicked()), this, SLOT(editMe()));
  setAlignment( int( QLabel::WordBreak | QLabel::AlignLeft ) );
}


QString DateBookEventLater::ampmTime(QTime tm) {

    QString s;
    if( ampm ) {
	int hour = tm.hour();
	if (hour == 0)
	    hour = 12;
	if (hour > 12)
	    hour -= 12;
	s.sprintf( "%2d:%02d %s", hour, tm.minute(),
		   (tm.hour() >= 12) ? "PM" : "AM" );
        return s;
    } else {
	s.sprintf( "%2d:%02d", tm.hour(), tm.minute() );
        return s;
    }

}


void DateBookEvent::editMe() {
  emit editEvent(event.event());
}

void DateBookEventLater::editMe() {
  emit editEvent(event.event());
}


