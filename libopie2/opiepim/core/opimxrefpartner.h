/*
                             This file is part of the Opie Project
                             Copyright (C) The Main Author <main-author@whereever.org>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/
#ifndef OPIM_XREF_PARTNER_H
#define OPIM_XREF_PARTNER_H

/* QT */
#include <qstring.h>

namespace Opie {
/**
 * This class represents one partner
 * of a Cross Reference.
 * In Opie one application
 * can link one uid
 * with one tableId( fieldId ) to another.
 */
class OPimXRefPartner {

  public:
    OPimXRefPartner( const QString& service = QString::null,
                     int uid = 0, int field = -1 );
    OPimXRefPartner( const OPimXRefPartner& );
    OPimXRefPartner& operator=( const OPimXRefPartner& );
    ~OPimXRefPartner();

    bool operator==(const OPimXRefPartner& );

    QString service()const;
    int uid()const;
    int field()const;

    void setService( const QString& service );
    void setUid( int uid );
    void setField( int field );
    
  private:
    QString m_app;
    int m_uid;
    int m_field;

    class Private;
    Private* d;
};

}

#endif
