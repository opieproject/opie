/* -*- C++ -*-
 *
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: connect.h,v 1.1 2003-05-23 19:43:46 tille Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 * based on EzPPP:
 * Copyright (C) 1997  Jay Painter
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _CONNECT_H_
#define _CONNECT_H_

#include <qtimer.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qevent.h>

#include "kpppconfig.h"
#include "pwentry.h"
//#include "docking.h"
//#include "loginterm.h"

#define MAXLOOPNEST (MAX_SCRIPT_ENTRIES/2)

class PPPStats;

class ConnectWidget : public QWidget {
  Q_OBJECT
public:
  ConnectWidget(QWidget *parent, const char *name);
  ~ConnectWidget();

public:
  void set_con_speed_string();
  void setMsg(const QString &);
  void pppdDied();

protected:
  void timerEvent(QTimerEvent *);
  void closeEvent( QCloseEvent *e );

private slots:
  void readChar(unsigned char);
  void pause();
  void if_waiting_slot();

public slots:
  void init();
  void preinit();
  void script_timed_out();
  void if_waiting_timed_out();
  void cancelbutton();

signals:
  void if_waiting_signal();
  void debugMessage(const QString &);
  void toggleDebugWindow();
  void closeDebugWindow();
  void debugPutChar(unsigned char);
  void startAccounting();
  void stopAccounting();

public:
  QString myreadbuffer;  // we want to keep every thing in order to fish for the

  // connection speed later on
  QPushButton *debug;
  int main_timer_ID;

private:
  int vmain;
  int substate;
  int scriptindex;
  QString scriptCommand, scriptArgument;
  QStringList *comlist, *arglist;

  //  static const int maxloopnest=(MAX_SCRIPT_ENTRIES/2);
  int loopnest;
  int loopstartindex[MAXLOOPNEST];
  bool loopend;
  QString loopstr[MAXLOOPNEST];

  bool semaphore;
  QTimer *inittimer;

  QTimer *timeout_timer;
  bool execppp();
  void writeline(const QString &);
  void checkBuffers();

  void setExpect(const QString &);
  bool expecting;
  QString expectstr;

  QString readbuffer;

  void setScan(const QString &);
  QString scanvar;
  QString scanstr;
  QString scanbuffer;
  bool scanning;

  bool pausing;
  PWEntry *prompt;
//  LoginTerm *termwindow;

  int scriptTimeout;
  QTimer *pausetimer;
  QTimer *if_timer;
  QTimer *if_timeout_timer;

  QLabel *messg;
  QPushButton *cancel;

  bool firstrunID;
  bool firstrunPW;

  unsigned int dialnumber; // the current number to dial

//  PPPStats *stats;
};


// non-member function to kill&wait on the pppd child process
extern void killppp();
void adddns();
void addpeerdns();
void removedns();
void add_domain(const QString & newdomain);
void auto_hostname();

#endif

