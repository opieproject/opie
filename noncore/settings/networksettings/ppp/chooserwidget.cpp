/*
 *           kPPP: A pppd front end for the KDE project
 *
 * $Id: chooserwidget.cpp,v 1.1.2.1 2003-07-30 17:54:23 tille Exp $
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

#include <qdir.h>
#include <stdlib.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qtabdialog.h>
#include <qwhatsthis.h>
#include <qmessagebox.h>

#include <qapplication.h>
#include <qbuttongroup.h>
#include <qmessagebox.h>
#include <qvgroupbox.h>

#include "accounts.h"
#include "authwidget.h"
#include "pppdata.h"
#include "edit.h"

void parseargs(char* buf, char** args);

ChooserWidget::ChooserWidget( PPPData *pd, QWidget *parent, const char *name )
    : QWidget( parent, name )//, _pppdata(pd)
{
    _pppdata = pd;
  QVBoxLayout *l1 = new QVBoxLayout(this, 10, 10);
  listListbox = new QListBox(this);

  connect(listListbox, SIGNAL(highlighted(int)),
	  this, SLOT(slotListBoxSelect(int)));
  connect(listListbox, SIGNAL(selected(int)),this, SLOT(edit()));
  l1->addWidget(listListbox, 10);

  edit_b = new QPushButton(tr("&Edit..."), this);
  connect(edit_b, SIGNAL(clicked()), SLOT(edit()));
  l1->addWidget(edit_b);

  new_b = new QPushButton(tr("&New..."), this);
  connect(new_b, SIGNAL(clicked()), SLOT(create()));
  l1->addWidget(new_b);


  copy_b = new QPushButton(tr("Co&py"), this);
  connect(copy_b, SIGNAL(clicked()), SLOT(copy()));
  l1->addWidget(copy_b);

  delete_b = new QPushButton(tr("De&lete"), this);
  connect(delete_b, SIGNAL(clicked()), SLOT(remove()));
  l1->addWidget(delete_b);


  QHBoxLayout *l12 = new QHBoxLayout;
  l1->addStretch(1);
  l1->addLayout(l12);

  l1->activate();

}

