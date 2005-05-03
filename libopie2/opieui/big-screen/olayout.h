/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
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

#ifndef OLAYOUT_H
#define OLAYOUT_H

/* QT */

#include <qlayout.h>

namespace Opie {
namespace Ui {

class OAutoBoxLayout : public QBoxLayout
{
    Q_OBJECT
public:
    OAutoBoxLayout( QWidget *parent, int border=0, int space = -1, const char *name=0 );
    OAutoBoxLayout( QLayout *parentLayout, int space = -1, const char *name=0 );
    OAutoBoxLayout( int space = -1, const char *name=0 );
    ~OAutoBoxLayout();

private:
    void fixDirection();
};

}; // namespace Ui
}; // namespace Opie

#endif


