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

#include "olayout.h"

/* OPIE */
#include <opie2/odebug.h>

/* QT */
#include <qapplication.h>

namespace Opie {
namespace Ui {

OAutoBoxLayout::OAutoBoxLayout( QWidget *parent, int border, int space, const char *name )
               :QBoxLayout( parent, LeftToRight, border, space, name )
{
    fixDirection();
}


OAutoBoxLayout::OAutoBoxLayout( QLayout *parentLayout, int space, const char *name )
               :QBoxLayout( parentLayout, LeftToRight, space, name )
{
    fixDirection();
}


OAutoBoxLayout::OAutoBoxLayout( int space, const char *name )
               :QBoxLayout( LeftToRight, space, name )
{
    fixDirection();
}


void OAutoBoxLayout::fixDirection()
{
    QWidget* desktop = QApplication::desktop();
    int w = desktop->width();
    int h = desktop->height();
    if ( h > w )
    {
        odebug << "OAutoBoxLayout: h > w, setting orientation to TopToBottom" << oendl;
        setDirection( TopToBottom );
    }
}


OAutoBoxLayout::~OAutoBoxLayout()
{
}


}; // namespace Ui
}; // namespace Opie



