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
#include "minidom.h"
#include "TodoItem.h"

#include <qpe/datebookdb.h>
#include <qpe/timestring.h>
#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h> 
#include <qpe/qprocess.h>
#include <qpe/resource.h>

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

//#include <iostream.h>
//#include <unistd.h>
#include <stdlib.h>

int MAX_LINES_TASK;
int MAX_CHAR_CLIP;
int MAX_LINES_MEET;
int SHOW_LOCATION;
int SHOW_NOTES;
// show only later dates
int ONLY_LATER;
int AUTOSTART;
/* 
 *  Constructs a Example which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
Today::Today( QWidget* parent,  const char* name, WFlags fl )
  : TodayBase( parent, name, fl ) {
  QObject::connect( (QObject*)PushButton1, SIGNAL( clicked() ), this, SLOT(startConfig() ) );
  QObject::connect( (QObject*)TodoButton, SIGNAL( clicked() ), this, SLOT(startTodo() ) );
  QObject::connect( (QObject*)DatesButton, SIGNAL( clicked() ), this, SLOT(startDatebook() ) );
  QObject::connect( (QObject*)MailButton, SIGNAL( clicked() ), this, SLOT(startMail() ) );
 
  
  autoStart();
  draw();
}


void Today::autoStart() {
  Config cfg("today");
  cfg.setGroup("Autostart"); 
  AUTOSTART = cfg.readNumEntry("autostart",1);
  if (AUTOSTART) {
    QCopEnvelope e("QPE/System", "autoStart(QString,QString)");
    e << QString("add");
    e << QString("today");
  } else {
    QCopEnvelope e("QPE/System", "autoStart(QString,QString)");
    e << QString("remove");
    e << QString("today");
  }
}

void Today::draw() {
  init();
  getDates();
  getMail();
  getTodo(); 
  // how often refresh
  QTimer::singleShot( 5*1000, this, SLOT(draw()) );
}

void Today::init() {
  QDate date = QDate::currentDate();
  QString time = (tr( date.toString()) );
//  QString time = (tr( date.toString()) , white);
  
  TextLabel1->setText(time);
  db = new DateBookDB;
  
  // read config
  Config cfg("today");
  cfg.setGroup("BaseConfig"); 

  // how many lines should be showed in the task section
  MAX_LINES_TASK = cfg.readNumEntry("maxlinestask",5);
  // after how many chars should the be cut off on tasks and notes
  MAX_CHAR_CLIP = cfg.readNumEntry("maxcharclip",30);
  // how many lines should be showed in the datebook section
  MAX_LINES_MEET = cfg.readNumEntry("maxlinesmeet",5);
  // If location is to be showed too, 1 to activate it.
  SHOW_LOCATION = cfg.readNumEntry("showlocation",1);
  // if notes should be shown 
  SHOW_NOTES = cfg.readNumEntry("shownotes",0);
  ONLY_LATER = cfg.readNumEntry("onlylater",1);

}

void Today::startConfig() {
  conf = new todayconfig ( this, "", true );
  
  
  //Config cfg = new Config("today");

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
  
  conf->CheckBoxAuto->setChecked(AUTOSTART);

  conf->exec();
  
  int maxlinestask = conf->SpinBox2->value();
  int maxmeet = conf->SpinBox1->value();
  int location = conf->CheckBox1->isChecked();
  int notes = conf->CheckBox2->isChecked();
  int maxcharclip = conf->SpinBox7->value();
  int onlylater = conf->CheckBox3->isChecked();
  int autostart =conf->CheckBoxAuto->isChecked();
  
  cfg.writeEntry("maxlinestask",maxlinestask);
  cfg.writeEntry("maxcharclip", maxcharclip); 
  cfg.writeEntry("maxlinesmeet",maxmeet);
  cfg.writeEntry("showlocation",location);
  cfg.writeEntry("shownotes", notes);
  cfg.writeEntry("onlylater", onlylater);
  cfg.setGroup("Autostart");
  cfg.writeEntry("autostart", autostart);
  // sync it to "disk"
  cfg.write(); 
  autoStart();
  draw();
}


/*
 *  Get all events that are in the datebook xml file for today
 */
void Today::getDates() {
  QDate date = QDate::currentDate();
  QTime time = QTime::currentTime();
  QValueList<EffectiveEvent> list = db->getEffectiveEvents(date, date);

  Config config( "qpe" );
  // if 24 h format
  //bool ampm = config.readBoolEntry( "AMPM", TRUE );
  
  int count=0;
  
  if ( list.count() > 0 ) {
    QString msg;
    
    for ( QValueList<EffectiveEvent>::ConstIterator it=list.begin();
    it!=list.end(); ++it ) {
      
      count++;
      
      if ( count <= MAX_LINES_MEET ) {
  //cout << time.toString() << endl;
  //cout << TimeString::dateString((*it).event().end()) << endl;
  
  // decide if to get all day or only later appointments
  if (!ONLY_LATER) {
    msg += "<B>" + (*it).description() + "</B>";
    if ( (*it).event().hasAlarm() ) {
      msg += " <b>[with alarm]</b>";
    }
    // include location or not
    if (SHOW_LOCATION == 1) {
      msg += "<BR><i>" + (*it).location();
      msg += "</i>";
    }
    
    if ( (TimeString::timeString(QTime((*it).event().start().time()) ) == "00:00") &&  (TimeString::timeString(QTime((*it).event().end().time()) ) == "23:59") ) {
      msg += "<br>All day";
    }  else {
      // start time of event
      msg += "<br>" + TimeString::timeString(QTime((*it).event().start().time()) )
        // end time of event
        + "<b> - </b>" + TimeString::timeString(QTime((*it).event().end().time()) );
    }
    msg += "<BR>";
    // include possible note or not
    if (SHOW_NOTES == 1) {
      msg += " <i>note</i>:" +((*it).notes()).mid(0, MAX_CHAR_CLIP) + "<br>";
    }
  } else if ((time.toString() <= TimeString::dateString((*it).event().end())) ) {
    msg += "<B>" + (*it).description() + "</B>";
    if ( (*it).event().hasAlarm() ) {
      msg += " <b>[with alarm]</b>";
    }
    // include location or not
    if (SHOW_LOCATION == 1) {
      msg+= "<BR><i>" + (*it).location();
      msg += "</i>";
    }
    
    if ( (TimeString::timeString(QTime((*it).event().start().time()) ) == "00:00") &&  (TimeString::timeString(QTime((*it).event().end().time()) ) == "23:59") ) {
      msg += "<br>All day";
    } else {
      // start time of event
      msg += "<br>" + TimeString::timeString(QTime((*it).event().start().time()) )
        // end time of event
        + "<b> - </b>" + TimeString::timeString(QTime((*it).event().end().time()) );
    }
    msg += "<BR>";
    // include possible note or not
    if (SHOW_NOTES == 1) {
      msg += " <i>note</i>:" +((*it).notes()).mid(0, MAX_CHAR_CLIP) + "<br>";
    }
  }
      }
    }
    if (msg.isEmpty()) {
      msg = tr("No more appointments today");
    }
    DatesField->setText(msg);
  }
}

/*
 * Parse in the todolist.xml
 */
QList<TodoItem> Today::loadTodo(const char *filename) {
  DOM *todo;
  ELE *tasks;
  ELE **tasklist;
  ATT **attlist;
  int i, j;
  char *description;
  int completed;
  int priority;
  TodoItem *tmp;
  QList<TodoItem> loadtodolist;
  
  todo = minidom_load(filename);
  
  tasks = todo->el;
  tasks = tasks->el[0]; /*!DOCTYPE-quickhack*/
  if(tasks) {
    tasklist = tasks->el;
    i = 0;
    while((tasklist) && (tasklist[i])) {
      attlist = tasklist[i]->at;
      j = 0;
      description = NULL;
      priority = -1;
      completed = -1;
      while((attlist) && (attlist[j])) {
  if(!attlist[j]->name) {
    continue;
  }
  if(!strcmp(attlist[j]->name, "Description")) {
    description = attlist[j]->value;
  }
  // get Completed tag (0 or 1)
  if(!strcmp(attlist[j]->name, "Completed")) {
    QString s = attlist[j]->name;
    if(s == "Completed") {
      completed = QString(attlist[j]->value).toInt();
    }
  }
  // get Priority (1 to 5)
  if(!strcmp(attlist[j]->name, "Priority")) {
    QString s = attlist[j]->name;
    if(s == "Priority") {
      priority = QString(attlist[j]->value).toInt(); 
    }
  }
  j++;
      }
      if(description) {
  tmp = new TodoItem(description, completed, priority);
  loadtodolist.append(tmp);
      }
      i++;
    }
  }

  minidom_free(todo);
  
  return loadtodolist;
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
 *
 */
void Today::getTodo() {
  QString output;
  QString tmpout;
  int count = 0;
  
  QDir dir;
  QString homedir = dir.homeDirPath (); 
  // see if todolist.xml does exist.
  QFile f(homedir +"/Applications/todolist/todolist.xml");
  if ( f.exists() ) {
    QList<TodoItem> todolist = loadTodo(homedir +"/Applications/todolist/todolist.xml");
    
    TodoItem *item;
    for( item = todolist.first(); item; item = todolist.next()) {
      if (!(item->getCompleted() == 1) ) {
  count++;
  if (count <= MAX_LINES_TASK) {
    tmpout += "<b>- </b>" + QString(((item)->getDescription().mid(0, MAX_CHAR_CLIP) + ("<br>")));
  }
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
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Today::~Today() {
  // no need to delete child widgets, Qt does it all for us
}
