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
**********************************************************************/

#ifndef __DESKTOP_H__
#define __DESKTOP_H__


#include "shutdownimpl.h"

#include <qpe/qpeapplication.h>

#include <qwidget.h>
#include <qdatetime.h>

class Background;
class Launcher;
class TaskBar;
class PowerStatus;
class QCopBridge;
class TransferServer;
class DesktopPowerAlerter;
class PackageSlave;
class QPEScreenSaver;

class DesktopApplication : public QPEApplication
{
  Q_OBJECT
public:
  DesktopApplication( int& argc, char **argv, Type t );
  ~DesktopApplication();

  static void switchLCD ( bool on ); // only for togglePower in Desktop

signals:
  void menu();
  void home();
  void datebook();
  void contacts();
  void launch();
  void email();
  void backlight();
  void power();
  void symbol();
  void numLockStateToggle();
  void capsLockStateToggle();
  void prepareForRestart();

protected:
#ifdef Q_WS_QWS

  bool qwsEventFilter( QWSEvent * );
#endif

  void shutdown();
  void restart();

public slots:
  virtual void desktopMessage ( const QCString &msg, const QByteArray &data );
  virtual void systemMessage ( const QCString &msg, const QByteArray &data );

protected slots:
  void shutdown( ShutdownImpl::Type );
  void psTimeout();
  void sendCard();
private:
  DesktopPowerAlerter *pa;
  PowerStatus *ps;
  QTimer *cardSendTimer;
  QCopChannel *channel;
  QPEScreenSaver *m_screensaver;
};


class Desktop : public QWidget
{
  Q_OBJECT
public:
  Desktop();
  ~Desktop();

  static bool screenLocked();

  void show();
  void checkMemory();

  void keyClick();
  void screenClick();
  static void soundAlarm();

public slots:
  void raiseDatebook();
  void raiseContacts();
  void raiseMenu();
  void raiseLauncher();
  void raiseEmail();
  void execAutoStart();
  void togglePower();
  void toggleLight();
  void toggleNumLockState();
  void toggleCapsLockState();
  void toggleSymbolInput();
  void terminateServers();
  void rereadVolumes();

  void home ( );

protected:
  void executeOrModify( const QString& appLnkFile );
  void styleChange( QStyle & );
  void timerEvent( QTimerEvent *e );

  QWidget *bg;
  Launcher *launcher;
  TaskBar *tb;

private:
  void startTransferServer();
  bool recoverMemory();

  QCopBridge *qcopBridge;
  TransferServer *transferServer;
  PackageSlave *packageSlave;

  QDateTime suspendTime;
  bool keyclick, touchclick, alarmsound;
};


#endif // __DESKTOP_H__

