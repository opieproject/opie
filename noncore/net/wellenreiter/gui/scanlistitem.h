/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef SCANLISTITEM_H
#define SCANLISTITEM_H

#include <qlistview.h>

class QString;

class MScanListItem: public QListViewItem
{
  public:

    MScanListItem::MScanListItem( QListView* parent,
                                  QString type,
                                  QString essid,
                                  QString ap,
                                  bool wep,
                                  int channel,
                                  int signal );

    MScanListItem::MScanListItem( QListViewItem* parent,
                                  QString type,
                                  QString essid,
                                  QString ap,
                                  bool wep,
                                  int channel,
                                  int signal );


  protected:

    virtual void decorateItem( QString type, QString essid, QString ap, bool wep, int channel, int signal );

};

#endif

