/* -*- C++ -*-
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: chooserwidget.h,v 1.1.2.1 2003-07-30 17:54:23 tille Exp $
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

#ifndef _CHOOSERWIDGET_H_
#define _CHOOSERWIDGET_H_

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
class AuthWidget;
class IPWidget;
class DNSWidget;
class GatewayWidget;
class PPPData;

class ChooserWidget : public QWidget {
  Q_OBJECT
public:
  ChooserWidget( PPPData *pd, QWidget *parent=0, const char *name=0 );
  ~ChooserWidget() {}

private slots:
  virtual void edit()  = 0;
  virtual void copy()  = 0;
  virtual void create()= 0;
  virtual void remove()= 0;
  virtual void slotListBoxSelect(int) = 0;


protected:
    PPPData *_pppdata;

    QListBox *listListbox;
    QPushButton *edit_b;
    QPushButton *copy_b;
    QPushButton *new_b;
    QPushButton *delete_b;
};


#endif

