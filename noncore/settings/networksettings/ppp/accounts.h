/* -*- C++ -*-
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: accounts.h,v 1.3 2003-06-02 14:10:31 tille Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 * based on EzPPP:
 * Copyright (C) 1997  Jay Painter
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

#ifndef _ACCOUNTS_H_
#define _ACCOUNTS_H_

#include <qwidget.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qlistbox.h>
//#include "acctselect.h"

class QDialog;
class QCheckBox;
class QLineEdit;
class QTabWidget;
class DialWidget;
class ScriptWidget;
class IPWidget;
class DNSWidget;
class GatewayWidget;
class PPPData;

class AccountWidget : public QWidget {
  Q_OBJECT
public:
  AccountWidget( PPPData *pd, QWidget *parent=0, const char *name=0 );
  ~AccountWidget() {}

private slots:
  void editaccount();
  void copyaccount();
  void newaccount();
  void deleteaccount();
  void slotListBoxSelect(int);

private:
  int doTab();

signals:
  void resetaccounts();

private:
    QString prettyPrintVolume(unsigned int);

    PPPData *_pppdata;
    QTabWidget *tabWindow;
    DialWidget *dial_w;
    IPWidget *ip_w;
    DNSWidget *dns_w;
    GatewayWidget *gateway_w;
    ScriptWidget *script_w;

    QListBox *accountlist_l;
    QPushButton *edit_b;
    QPushButton *copy_b;
    QPushButton *new_b;
    QPushButton *delete_b;
};


// class QueryReset : public QDialog {
//   Q_OBJECT
// public:
//   QueryReset(QWidget *parent);

//   enum {COSTS=1, VOLUME=2};

// private slots:
//   void accepted();

// private:
//   QCheckBox *costs, *volume;
// };

#endif

