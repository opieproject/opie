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
#ifndef COMBO_BOX_HELPER_H
#define COMBO_BOX_HELPER_H

#include <qstring.h>


class QComboBox;


/**
 * \brief ComboboxHelper with a small Helper for Configuring it
 *
 * ComboboxHelper contains  methods for helping managing
 * Comboboxes with 'dynamic' data.
 * All Methods related to this class are static and operate
 * on a QComboBox.
 */
struct ComboboxHelper {
    static void setCurrent(const QString&, QComboBox *);
};

#endif
