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

#include <qpe/qpeapplication.h>
#ifdef QWS
#include <qpe/qcopenvelope_qws.h>
#endif
#include <qapplication.h>
#include <qpe/pim/addressbookaccess.h>
#include "addressbookdumper.h"
#include <qvbox.h>
#include <qpushbutton.h>

int main( int argc, char ** argv )
{
  QPEApplication a( argc, argv );
  QVBox* vbox = new QVBox(0L);
  QPushButton* clicker = new QPushButton("Refresh", vbox);
  QPushButton* startBigEditButton = new QPushButton("Start Big Edit", vbox);
  QPushButton* endBigEditButton = new QPushButton("End Big Edit", vbox);
  QPushButton* addContactButton = new QPushButton("Add Contact", vbox);
  AddressBookDumper* abDumper = new AddressBookDumper(vbox);
  QObject::connect(clicker, SIGNAL(clicked()), abDumper, SLOT(abChanged()));
  QObject::connect(startBigEditButton, SIGNAL(clicked()), abDumper, SLOT(startBigEdit()));
  QObject::connect(endBigEditButton, SIGNAL(clicked()), abDumper, SLOT(endBigEdit()));
  QObject::connect(addContactButton, SIGNAL(clicked()), abDumper, SLOT(addContact()));
  a.setMainWidget(vbox);
  vbox->show();
  return a.exec();
}
