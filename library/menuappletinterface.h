/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
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

#ifndef MENUAPPLETINTERFACE_H
#define MENUAPPLETINTERFACE_H

#include <qpe/qcom.h>
#include <qiconset.h>

#ifndef QT_NO_COMPONENT
// {9bb81198-3061-46fc-a7bd-d14cd065836d}
# ifndef IID_MenuApplet
#  define IID_MenuApplet QUuid(0x9bb81198, 0x3061, 0x46fc, 0xa7, 0xbd, 0xd1, 0x4c, 0xd0, 0x65, 0x83, 0x6d)
# endif
#endif

class QPopupMenu;

struct MenuAppletInterface : public QUnknownInterface
{
    virtual QString name ( ) const = 0;
    
    virtual int position ( ) const = 0;
    virtual QIconSet icon ( ) const = 0;
    virtual QString text ( ) const = 0;
    
    virtual QPopupMenu *popup ( QWidget *parent ) const = 0;

	// callback for popup() == 0    
    virtual void activated ( ) = 0;
};

#endif
