/* -*- C++ -*-
 *
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: conwindow.h,v 1.2.2.1 2003-07-15 15:29:08 tille Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
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

#ifndef _CONWINDOW_H_
#define _CONWINDOW_H_


#include <qtimer.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qframe.h>
#include <qevent.h>
#include <qlayout.h>

class PPPData;

class ConWindow : public QWidget {
Q_OBJECT

public:
  ConWindow(PPPData*, QWidget *parent, const char *name, QDialog* );
  ~ConWindow();

protected:
  void closeEvent( QCloseEvent *e );
  bool event( QEvent *e );

private slots:
  void timeclick();
//  void dock();

public:
  void setConnectionSpeed(const QString&);
  void startClock();
  void stopClock();
//  void accounting(bool); // show/ hide accounting info

public slots:
  void slotAccounting(QString, QString);

private:
  QLabel *info1;
  QLabel *info2;
  QPushButton *cancelbutton;
  QPushButton *statsbutton;
  QString prettyPrintVolume(unsigned int);
  int minutes;
  int seconds;
  int hours;
  int days;
  QFrame *fline;
  QLabel *timelabel1;
  QLabel *timelabel2;
  QLabel *total_bill, *total_bill_l;
  QLabel *session_bill, *session_bill_l;
  QString caption_string;
  QString time_string2;
  QString time_string;
  QTimer *clocktimer;
  QVBoxLayout *tl1;
  QLabel *vollabel;
  QLabel *volinfo;
    PPPData *_pppdata;
};


#endif





