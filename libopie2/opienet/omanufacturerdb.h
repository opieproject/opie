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

/**
 * @brief A Ethernet card vendor database.
 *
 * This class encapsulates the lookup of Ethernet vendor given a
 * certain Mac Address. Only the first three bytes define the vendor.
 */
class OManufacturerDB
{
  public:
    /**
     * @returns the one-and-only @ref OManufacturerDB instance.
     */
    static OManufacturerDB* instance();
    /**
     * @returns the short manufacturer string given a @a macaddr.
     */
    const QString& lookup( const QString& macaddr ) const;
    /**
     * @returns the enhanced manufacturer string given a @a macaddr.
     */
    const QString& lookupExt( const QString& macaddr ) const;

  protected:
    OManufacturerDB();
    virtual ~OManufacturerDB();

  private:
    QMap<QString, QString> manufacturers;
    QMap<QString, QString> manufacturersExt;
    static OManufacturerDB* _instance;
};

#endif

