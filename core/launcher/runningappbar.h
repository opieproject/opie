/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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
**********************************************************************/

#ifndef RUNNING_APP_BAR_H
#define RUNNING_APP_BAR_H

#include <qframe.h>
#include <qlist.h>
#include <qtimer.h>

class AppLnk;
class AppLnkSet;
class QCString;
class QProcess;
class QMessageBox;

class RunningAppBar : public QFrame {
  Q_OBJECT

 public:
  RunningAppBar(QWidget* parent);
  ~RunningAppBar();
  
  void addTask(const AppLnk& appLnk);
  void removeTask(const AppLnk& appLnk);
  void paintEvent(QPaintEvent* event);
  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  QSize sizeHint() const;

 private slots:
   void newQcopChannel(const QString& channel);
   void removedQcopChannel(const QString& channel);
 void received(const QCString& msg, const QByteArray& data);

 private:
  AppLnkSet* m_AppLnkSet;
  QList<AppLnk> m_AppList;
  int m_SelectedAppIndex;
  int spacing;
};

/**
 * Internal class that checks back in on the process when timerExpired is called
 * to make sure the process is on top.  If it's not it displays a dialog
 * box asking permission to kill it.
 */
class AppMonitor : public QObject {
  Q_OBJECT
    
 public:
  static const int RAISE_TIMEOUT_MS;

  AppMonitor(const AppLnk& app, RunningAppBar& owner);
  ~AppMonitor();
  
  private slots:
    void timerExpired();
    void received(const QCString& msg, const QByteArray& data);
    void psProcFinished();

 private:
  RunningAppBar& m_Owner;
  const AppLnk& m_App;
  QTimer m_Timer;
  QProcess* m_PsProc;
  QMessageBox* m_AppKillerBox;
};

#endif

