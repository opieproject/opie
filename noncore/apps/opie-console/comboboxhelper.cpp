/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002,2004 Holger Hans Peter Freyther <freyther@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this library; see the file COPYING.BIN.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "comboboxhelper.h"

#include <qstring.h>
#include <qcombobox.h>

/**
 * \brief Set string to be Current String inside the QCombox without duplicating it
 *
 * This method will make \par str the current QString. If
 * the QString is already inside the QComboBox it will be
 * set current. If it isn't it will be added.
 *
 * @param str The QString to be set current
 * @param bo  The QComboBox to operate on
 */
void ComboboxHelper::setCurrent( const QString& str, QComboBox* bo ) {
    const uint b = bo->count();
    for (uint i = 0; i < b; i++ ) {
        if ( bo->text(i) == str ) {
            bo->setCurrentItem( i );
            return;
        }
    }

    bo->insertItem( str );
    bo->setCurrentItem( b );
}
