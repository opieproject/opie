/*
 *            kPPP: A pppd front end for the KDE project
 *
 * $Id: pppdargs.h,v 1.1 2003-05-23 19:43:46 tille Exp $
 *
 *            Copyright (C) 1997 Bernd Johannes Wuebben
 *                   wuebben@math.cornell.edu
 *
 * based on EzPPP:
 * Copyright (C) 1997  Jay Painter
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _PPPDARGS_H_
#define _PPPDARGS_H_

#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlabel.h>

class PPPdArguments : public QDialog {
Q_OBJECT
public:
  PPPdArguments(QWidget *parent=0, const char *name=0);
  ~PPPdArguments() {}

private slots:
  void addbutton();
  void removebutton();
  void defaultsbutton();
  void closebutton();
  void textChanged(const QString &);
  void itemSelected(int);

private:
  void init();

  QLabel *argument_label;

  QLineEdit *argument;

  QPushButton *add;
  QPushButton *remove;
  QPushButton *defaults;

  QListBox *arguments;

  QPushButton *closebtn;
};
#endif










