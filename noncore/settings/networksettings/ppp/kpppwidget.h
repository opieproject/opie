/*
 *
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: kpppwidget.h,v 1.5 2003-08-09 17:14:55 kergoth Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 *            Copyright (C) 1998-2002 Harri Porten <porten@kde.org>
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

#ifndef __KPPPWIDGET_H__
#define __KPPPWIDGET_H__

#include <qstring.h>

#include "accounts.h"
#include "connect.h"
#include "conwindow.h"

class QPushButton;
class Interface;
class PPPData;

class KPPPWidget : public QDialog {
  Q_OBJECT
public:

  KPPPWidget(PPPData*, Interface*, QWidget *parent=0, const char *name=0, bool modal = false, WFlags fl = 0 );
  ~KPPPWidget();

  void setPW_Edit(const QString &);


private slots:
  void newdefaultaccount(int);
  void beginConnect();
  void quitbutton();
  void usernameChanged(const QString &);
  void passwordChanged(const QString &);
  void enterPressedInID();
  void enterPressedInPW();
  void saveMyself();
  void shutDown();


public slots:
  void disconnect();
  void log_window_toggled(bool on);

signals:
  void begin_connect();
  void cmdl_start();

public:
  QCheckBox *log;
  bool connected;
  QString con_speed;
  ConnectWidget *con;
  ConWindow *con_win;
  //  PPPStatsDlg *statdlg;
  // AccountingBase *acct;
  QPushButton *quit_b;
  //PPPStats *stats;

private:
  // void prepareSetupDialog();
  void interruptConnection();
  void sigChld();
  void sigPPPDDied();
  QString encodeWord(const QString &s);
  void showNews ();

  QString ruleset_load_errmsg;
  PPPData *_pppdata;
  QPushButton *setup_b;
  QFrame *fline;
  QFrame *fline1;
  QPushButton *connect_b;
  QComboBox *connectto_c;
  QLabel *ID_Label;
  QLabel *PW_Label;
  QLineEdit *ID_Edit;
  QLineEdit *PW_Edit;
  QLabel *label1;
  QLabel *label2;
  QLabel *label3;
  QLabel *label4;
  QLabel *label5;
  QLabel *label6;
  QLabel *radio_label;

/*   QString m_strCmdlAccount; */
/*   bool m_bQuitOnDisconnect; */
/*   bool m_bCmdlAccount; */
};


#endif

