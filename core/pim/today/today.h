/*
 * today.h
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


#ifndef TODAY_H
#define TODAY_H

#include <qpe/datebookdb.h>
#include <qpe/event.h>

#include <qdatetime.h>
#include <qlist.h> 

#include "TodoItem.h"
#include "todayconfig.h"
#include "todaybase.h"
#include "clickablelabel.h"

class QVBoxLayout;

class Today : public TodayBase {
  Q_OBJECT
    
    public:
  Today( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~Today();
  
  private slots:
    void startConfig();
    void startTodo();
    void startDatebook();
    void startMail();
    void draw();
    void editEvent(const Event &e);
 private:
    void init();
    void getDates();
    void getTodo();
    void getMail();
    void autoStart();
    bool checkIfModified();
    void setOwnerField();
    void setOwnerField(QString &string);
    QList<TodoItem> loadTodo(const char *filename);
private slots:
    void channelReceived(const QCString &msg, const QByteArray & data);

 private:
    DateBookDB *db;
    todayconfig *conf;
    QWidget* AllDateBookEvents;
    //Config cfg;
    int MAX_LINES_TASK;
    int MAX_CHAR_CLIP;
    int MAX_LINES_MEET;
    int SHOW_LOCATION; 
    int SHOW_NOTES;
};

class DateBookEvent: public ClickableLabel {
  Q_OBJECT
public:
    DateBookEvent(const EffectiveEvent &ev, 
			 QWidget* parent = 0, const char* name = 0, 
			 WFlags fl = 0);
signals:
    void editEvent(const Event &e);
private slots:
    void editMe();
private:
    const EffectiveEvent event;
};

class DateBookEventLater: public ClickableLabel {
  Q_OBJECT
public:
    DateBookEventLater(const EffectiveEvent &ev, 
			 QWidget* parent = 0, const char* name = 0, 
			 WFlags fl = 0);
signals:
    void editEvent(const Event &e);
private slots:
    void editMe();
private:
    const EffectiveEvent event;
};

#endif // TODAY_H

