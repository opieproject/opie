/* -*- C++ -*-
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: accounts.h,v 1.4.2.2 2003-07-30 15:05:58 harlekin Exp $
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

#include <qdialog.h>
#include <qpushbutton.h>
#include <qlistbox.h>
//#include "acctselect.h"

class QDialog;
class QCheckBox;
class QLineEdit;
class QTabWidget;
class DialWidget;
class AuthWidget;
class IPWidget;
class DNSWidget;
class GatewayWidget;
class PPPData;

class AccountWidget : public QDialog {
  Q_OBJECT
public:
  AccountWidget( PPPData *pd, QWidget *parent=0, const char *name=0, bool modal=0, WFlags f=0 );
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
    AuthWidget *auth_w;

    QListBox *accountlist_l;
    QPushButton *edit_b;
    QPushButton *copy_b;
    QPushButton *new_b;
    QPushButton *delete_b;
};


#endif

