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

#include <qpushbutton.h>
#include <qlabel.h>

#include <qpe/datebookdb.h>
#include <qpe/timestring.h>
#include <qpe/config.h>

#include <qpe/qprocess.h>
#include <qdir.h>
#include <qfile.h>
#include <qdatetime.h> 
#include <qtextstream.h>
#include <qcheckbox.h>
#include <qspinbox.h>

//#include <iostream.h>
//#include <unistd.h>
#include <stdlib.h>

int MAX_LINES_TASK;
int MAX_CHAR_CLIP;
int MAX_LINES_MEET;
int SHOW_LOCATION;
int SHOW_NOTES;
// show only later dates
int ONLY_LATER = 1;
/* 
 *  Constructs a Example which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
Today::Today( QWidget* parent,  const char* name, WFlags fl )
  : TodayBase( parent, name, fl )
{
  QObject::connect( (QObject*)PushButton1, SIGNAL( clicked() ), this, SLOT(startConfig() ) );
  QObject::connect( (QObject*)TodoButton, SIGNAL( clicked() ), this, SLOT(startTodo() ) );
  QObject::connect( (QObject*)DatesButton, SIGNAL( clicked() ), this, SLOT(startDatebook() ) );

  QDate date = QDate::currentDate();
  QString time = (date.toString());
  TextLabel1->setText(time);
  db = new DateBookDB;
 
  init();
  getDates();
  getTodo();
}


void Today::init()
{
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
}

void Today::startConfig() 
{
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
   
  conf->exec();
  
  int maxlinestask = conf->SpinBox2->value();
  int maxmeet = conf->SpinBox1->value();
  int location = conf->CheckBox1->isChecked();
  int notes = conf->CheckBox2->isChecked();
  int maxcharclip = conf->SpinBox7->value();
  
  cfg.writeEntry("maxlinestask",maxlinestask);
  cfg.writeEntry("maxcharclip", maxcharclip); 
  cfg.writeEntry("maxlinesmeet",maxmeet);
  cfg.writeEntry("showlocation",location);
  cfg.writeEntry("shownotes", notes);
  // sync it to "disk"
  cfg.write(); 
  
  init();
  getDates();
  getTodo();
  //cout << location << endl;
}


/*
 *  Get all events that are in the datebook xml file for today
 */
void Today::getDates()
{
  QDate date = QDate::currentDate();
  QTime time = QTime::currentTime();
  QValueList<EffectiveEvent> list = db->getEffectiveEvents(date, date);

  Config config( "qpe" );
  // if 24 h format
  //  bool ampm = config.readBoolEntry( "AMPM", TRUE );
  
  int count=0;
  
  if ( list.count() > 0 ) {
    QString msg;
    
    for ( QValueList<EffectiveEvent>::ConstIterator it=list.begin();
	  it!=list.end(); ++it ) {
      
      count++;
      
      if ( count <= MAX_LINES_MEET ) 
	{
	  //only get events past current time (start or end??)
	  //cout << time.toString() << endl;
	  //cout << TimeString::dateString((*it).event().end()) << endl;
	  // still some bug in here, 1 h off 
	  if ((time.toString()  <= TimeString::dateString((*it).event().end())) && ONLY_LATER )
	    {
	      msg += "<B>" + (*it).description() + "</B>";
	      // include location or not
	      if (SHOW_LOCATION == 1) 
		{
		  msg+= "<BR>" + (*it).location();
		}
	      msg += "<BR>"
		// start time of event
		+ TimeString::timeString(QTime((*it).event().start().time()) )
		// end time of event
		+ "<b> - </b>" + TimeString::timeString(QTime((*it).event().end().time()) )
		+ "<BR>";
	      // include possible note or not
	      if (SHOW_NOTES == 1)
		{
		  msg += " <i>note</i>:" +((*it).notes()).mid(0, MAX_CHAR_CLIP) + "<br>";
		}
	    }
	}
    }
    DatesField->setText(msg);
  }
}

/*
 *
 *
 */
QList<TodoItem> Today::loadTodo(const char *filename)
{
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
  if(tasks)
    {
      tasklist = tasks->el;
      i = 0;
      while((tasklist) && (tasklist[i]))
	{
	  attlist = tasklist[i]->at;
	  j = 0;
	  description = NULL;
	  priority = -1;
	  completed = -1;
	  while((attlist) && (attlist[j]))
 	    {
	      if(!attlist[i]->name) continue;
	      if(!strcmp(attlist[j]->name, "Description"))
		{
		  description = attlist[j]->value;
		}
	      // get Completed tag (0 or 1)
	      if(!strcmp(attlist[j]->name, "Completed"))
		{
		  QString s = attlist[j]->name;
		  if(s == "Completed")
		    {
		      completed = QString(attlist[j]->value).toInt();
		    }
		}
	      // get Priority (1 to 5)
	      if(!strcmp(attlist[j]->name, "Priority")) 
		{
		  QString s = attlist[j]->name;
		  if(s == "Priority")
		    {
		      priority = QString(attlist[j]->value).toInt(); 
		    }
		}
	      j++;
	    }
	  if(description)
	    {
	      tmp = new TodoItem(description, completed, priority);
	      loadtodolist.append(tmp);
	    }
	  i++;
	}
    }
  
  minidom_free(todo);
  
  return loadtodolist;
}


/*
 * Get the todos
 *
 */
void Today::getTodo()
{
  QString output;
  QString tmpout;
  int count = 0;
  
  QDir dir;
  QString homedir = dir.homeDirPath (); 
  // see if todolist.xml does exist.
  QFile f(homedir +"/Applications/todolist/todolist.xml");
  if ( f.exists() )
    {
      QList<TodoItem> todolist = loadTodo(homedir +"/Applications/todolist/todolist.xml");
      
      TodoItem *item;
      for( item = todolist.first(); item; item = todolist.next())
	{
	  if (!(item->getCompleted() == 1) )
	    {
	      count++;
	      if (count <= MAX_LINES_TASK)
		{
		  tmpout += "<b>- </b>" + QString(((item)->getDescription().mid(0, MAX_CHAR_CLIP) + ("<br>")));
		}
	    }
	}
    }
  
  if (count > 0) 
    {
      output = QString("There are <b> %1</b> active tasks: <br>").arg(count);
      output += tmpout;
    }
  else 
    {
      output = ("No active tasks");
    }
  
  TodoField->setText(output);
}

/*
 * lanches datebook
 */
void Today::startDatebook()
{
  //ugly but working
  system("/opt/QtPalmtop/bin/datebook");
}

/*
 * lanches todolist
 */ 
void Today::startTodo()
{
  //  QProcess *datelanch = new QProcess( this,  "datebook");
  //datelanch->start();
  system("/opt/QtPalmtop/bin/todolist");
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Today::~Today()
{
  // no need to delete child widgets, Qt does it all for us
}
