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

#ifndef OCONTACTFIELDS_H
#define OCONTACTFIELDS_H

class QStringList;

/* OPIE */
#include <opie2/opimcontact.h>

/* QT */
#include <qmap.h>
#include <qstring.h>

#define CONTACT_FIELD_ORDER_NAME "opie-contactfield-order"
#define DEFAULT_FIELD_ORDER "__________"

namespace Opie
{
class OPimContactFields
{

  public:
    OPimContactFields();
    ~OPimContactFields();
    /** Set the index for combo boxes.
     *  Sets the <b>index</b> of combo <b>num</b>.
     *  @param num selects the number of the combo
     *  @param index sets the index in the combo
     */
    void setFieldOrder( int num, int index );

    /** Get the index for combo boxes.
     * Returns the index of combo <b>num</b> or defindex
     * if none was defined..
     * @param num Selects the number of the combo
     * @param defIndex will be returned if none was defined (either
     * globally in the config file, nor by the contact which was used
     * by loadFromRecord() )
     */
    int getFieldOrder( int num, int defIndex );

    /** Store fieldorder to contact. */
    void saveToRecord( OPimContact& );
    /** Get Fieldorder from contact. */
    void loadFromRecord( const OPimContact& );

  private:
    QString fieldOrder;
    QString globalFieldOrder;
    bool changedFieldOrder;

  public:
    static QStringList personalfields( bool sorted = true, bool translated = false );
    static QStringList phonefields( bool sorted = true, bool translated = false );
    static QStringList detailsfields( bool sorted = true, bool translated = false );
    static QStringList fields( bool sorted = true, bool translated = false );

    static QStringList trpersonalfields( bool sorted = true );
    static QStringList untrpersonalfields( bool sorted = true );
    static QStringList trphonefields( bool sorted = true );
    static QStringList untrphonefields( bool sorted = true );
    static QStringList trdetailsfields( bool sorted = true );
    static QStringList untrdetailsfields( bool sorted = true );
    static QStringList trfields( bool sorted = true );
    static QStringList untrfields( bool sorted = true );

    static QMap<int, QString> idToTrFields();
    static QMap<QString, int> trFieldsToId();
    static QMap<int, QString> idToUntrFields();
    static QMap<QString, int> untrFieldsToId();

};
}
#endif
