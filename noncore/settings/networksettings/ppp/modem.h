/*
 *              kPPP: A pppd Front End for the KDE project
 *
 * $Id: modem.h,v 1.4 2003-05-30 15:06:17 tille Exp $
 *
 *              Copyright (C) 1997 Bernd Johannes Wuebben
 *                      wuebben@math.cornell.edu
 * 
 * This file was added by Harri Porten <porten@tu-harburg.de>
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

#ifndef _MODEM_H_
#define _MODEM_H_

#include <qdir.h>

#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include <qsocketnotifier.h>

class PPPData;

void    alarm_handler(int);
const char *pppdPath();

class Modem : public QObject {
  Q_OBJECT
public:
  Modem(PPPData*);
  ~Modem();

  const QString getDevice() {return device;};
  void setDevice(const QString dev) {device = dev;}; 

  bool opentty();
  bool closetty();
  bool hangup();
  bool writeChar(unsigned char);
  bool writeLine(const char *);
  bool dataMode() const { return data_mode; }
  void setDataMode(bool set) { data_mode = set; }
  const QString modemMessage();  
  speed_t modemspeed();
  static QString parseModemSpeed(const QString &);
  void notify(const QObject *, const char *);
  void stop();
  void flush();

  int     lockdevice();
  void    unlockdevice();

  bool setSecret(int,const char*,const char*);
  bool removeSecret(int);
  void killPPPDaemon();
  int  pppdExitStatus();
  bool execPPPDaemon(const QString & arguments);
  int  openResolv(int flags);
  bool setHostname(const QString & name);
  
public:
  enum Auth { PAP = 1, CHAP };
  //  static Modem *modem;
  int lastStatus;

signals:
  void charWaiting(unsigned char);

private slots:
  void startNotifier();
  void stopNotifier();
  void readtty(int);  

private:
  enum { MaxPathLen = 30, MaxStrLen = 40, MaxArgs = 100 };
  enum { Original=0x100, New=0x200, Old=0x400 } Version;

  const char* authFile(Auth method, int version = Original );
  bool createAuthFile(Auth method,const char *username,const char *password);
  bool removeAuthFile(Auth method);
  bool execpppd(const char *arguments);
  bool killpppd();
  void parseargs(char* buf, char** args);
  void escape_to_command_mode();
  int openLockfile(QString,int);

private:
  QString device;
  QString lockfile;   
  int modemfd;
  int pppdPid;
  int _pppdExitStatus;
  QSocketNotifier *sn;
  bool data_mode;
  QString errmsg;
  struct termios initial_tty;
  struct termios tty;
  bool modem_is_locked;
  PPPData *_pppdata;
};

#endif


