/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
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
#ifndef ADDRESSLIST_H
#define ADDRESSLIST_H

#include <qobject.h>
#include <qlist.h>

struct Contact
{
  QString email;
  QString name;
};

class AddressList : public QObject
{
  Q_OBJECT

public:
  AddressList();
  ~AddressList();
  void addContact(const QString &email, const QString &name);
  bool containsEmail(const QString &email);
  bool containsName(const QString &name);
  QString getNameByEmail(const QString &email);
  QString getEmailByName(const QString &name);
  QList<Contact>* getContactList();

private:
  int getEmailRef(const QString &email);
  int getNameRef(const QString &name);
  QString getRightString(const QString &in);
  void read();

private:
  QList<Contact> addresses;
  QString filename;
  bool dirty;
};

#endif
