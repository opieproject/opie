/***************************************************************************
                          qlinphone.h  -  description
                             -------------------
    begin                : sam mai 24 2003
    copyright            : (C) 2003 by Simon Morlat, 2004 Maximilian Reiss
    email                : simon.morlat@linphone.org, harlekin@handhelds.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "mainwidget.h"
#include <qobject.h>
#include <qpushbutton.h>
#include <qqueue.h>
#include <pthread.h>

class GuiTask {
public:
    GuiTask(QWidget *w) {
        _w=w;
    }
    virtual void execute()=0;
    virtual ~GuiTask() {}
    ;
protected:
    QWidget *_w;
};

class ShowTask: public GuiTask {
public:
ShowTask(QWidget *w) : GuiTask(w) {}
    ;
private:
    void execute() {
        _w->show();
    }
};

class DisplaySomethingTask : public GuiTask {
public:
DisplaySomethingTask(QWidget* w,QString &msg): GuiTask(w) {
        _msg=msg;
    }
protected:
    QString _msg;
};

class UpdateStatusBarTask : public DisplaySomethingTask {
public:
UpdateStatusBarTask(QWidget *w,QString &msg) :DisplaySomethingTask(w,msg) {}
    ;
private:
    void execute();
};

class InviteReceivedTask : public DisplaySomethingTask {
public:
InviteReceivedTask(QWidget *w,QString &msg) :DisplaySomethingTask(w,msg) {}
    ;
private:
    void execute();
};

class DisplayMessageTask : public DisplaySomethingTask {
public:
    enum MessageType{Info,Warn};
DisplayMessageTask(QWidget *w,QString &msg,MessageType type) : DisplaySomethingTask(w,msg) {
        _msgtype=type;
    };
private:
    void execute();
    MessageType _msgtype;
};



typedef struct _LinphoneCore LinphoneCore;

class MyMutex {
public:
    MyMutex() {
        pthread_mutex_init(&_mutex,NULL);
    }
    void lock() {
        pthread_mutex_lock(&_mutex);
    }
    void unlock() {
        pthread_mutex_unlock(&_mutex);
    }
    ~MyMutex() {
        pthread_mutex_destroy(&_mutex);
    }
private:
    pthread_mutex_t _mutex;
};

class QLinphoneMainWidget : public _QLinphoneMainWidget {

Q_OBJECT

public:
    QLinphoneMainWidget(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~QLinphoneMainWidget();

 
   void callOrAccept();
    void terminateCall();
    void inviteReceived(QString &from);
    void displayStatus(QString &status);
    void helpAbout();   
   // make it private
    void createLinphoneCore();
    void pushGuiTask(GuiTask* g);

private slots:
    void slotHide( bool show );

private:
    void readConfig();
    void writeConfig();
    void processGuiTasks();
    void timerEvent(QTimerEvent *);
    LinphoneCore *_core;
    QQueue<GuiTask> _actionq;
    MyMutex _mutex;
};


