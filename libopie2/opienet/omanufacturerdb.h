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

#ifndef OMANUFACTURERDB_H
#define OMANUFACTURERDB_H

#include <qmap.h>

class OManufacturerDB
{
  public:
    //FIXME make us consistent -zecke I use self(), sandman inst() you use instance() so we need to chose one!
    static OManufacturerDB* instance();
    const QString& lookup( const QString& macaddr ) const;

  protected:
    OManufacturerDB();
    virtual ~OManufacturerDB();

  private:
    QMap<QString, QString> manufacturers;
    static OManufacturerDB* _instance;
};

#endif

