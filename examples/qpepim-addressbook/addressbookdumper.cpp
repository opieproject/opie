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

#include <qpe/contact.h>
#include <qvaluelist.h>
#include "addressbookdumper.h"

AddressBookDumper::AddressBookDumper(QWidget* parent) : QMultiLineEdit(parent) {
  //  connect(&m_ABAccess, SIGNAL(addressbookUpdated()), this, SLOT(abChanged()));
}

AddressBookDumper::~AddressBookDumper() {
}

void AddressBookDumper::abChanged() {
  QString newText;
  QValueList<Contact> contacts = m_ABAccess.contacts();
  QValueListConstIterator<Contact> it;
  for (it = contacts.begin() ; it != contacts.end(); it++) {
    newText.append((*it).firstName() + " " + (*it).lastName() + "\n");
  }
  setText(newText);
}

void AddressBookDumper::startBigEdit() {
  if (m_ABAccess.startBlockEdit())
    qDebug("*** Block edit successfully started.");
  else
    qDebug("*** Block edit start failed.");
}

void AddressBookDumper::endBigEdit() {
  if (m_ABAccess.endBlockEdit())
    qDebug("*** Block edit successfully ended.");
  else
    qDebug("*** Block edit end failed.");
}

void AddressBookDumper::addContact() {
  Contact foo;
  foo.setFirstName("Foo");
  foo.setLastName("Bar");
  foo.setFileAs();
  if (m_ABAccess.addContact(foo))
    qDebug("*** Add succeeded..");
  else
    qDebug("*** Add failed..");
}
