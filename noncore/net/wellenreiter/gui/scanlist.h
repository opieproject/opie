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

#ifndef SCANLIST_H
#define SCANLIST_H

#include <qlistview.h>

class QString;

class ManufacturerDB;

class MScanListView: public QListView
{
  Q_OBJECT
  
  public:
    MScanListView( QWidget* parent = 0, const char* name = 0 );
    virtual ~MScanListView();
  
    void setManufacturerDB( ManufacturerDB* manufacturerdb );
  
  public slots:
    void addNewItem( QString type, QString essid, QString macaddr, bool wep, int channel, int signal );
    
  private:
    ManufacturerDB* _manufacturerdb;

};

#endif

