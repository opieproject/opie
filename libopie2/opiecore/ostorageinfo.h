/*
                             This file is part of the Opie Project
                             Copyright (C) 2004 Andreas 'ar' Richter <ar@oszine.de>
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

#ifndef OSTORAGE_H
#define OSTORAGE_H

#include <qpe/storage.h>

namespace Opie {
namespace Core {

class OStorageInfo : public StorageInfo
{
  Q_OBJECT

  public:

    OStorageInfo( QObject *parent=0 );
    ~OStorageInfo();

   /**
    * @returns the mount path of the CF (Compact Flash) card
    *
    **/
    QString cfPath() const;
   /**
    * @returns the mount path of the SD (Secure Digital) card
    *
    **/
    QString sdPath() const;
   /**
    * @returns the mount path of the MMC (MultiMedia) card
    *
    **/
    QString mmcPath() const;
private:
    class Private;
    Private *d;
};

}
}
#endif // OSTORAGE_H

