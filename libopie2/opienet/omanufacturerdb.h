/*
                             This file is part of the Opie Project
                             (C) 2003 Michael 'Mickey' Lauer <mickey@Vanille.de>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef OMANUFACTURERDB_H
#define OMANUFACTURERDB_H

#include <qmap.h>

namespace Opie {
namespace Net {

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
    class Private;
    Private *d;
};

}
}

#endif

