/*
                             This file is part of the Opie Project
                             Copyright (C) 2003 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#ifndef OGLOBAL_H
#define OGLOBAL_H

#include <opie2/oconfig.h>

//FIXME Is it wise or even necessary to inherit OGlobal from Global?
//      once we totally skip libqpe it should ideally swallow Global -zecke
//      You're right. I deleted global as the base class. -mickeyl

class OGlobal
{
  public:
    //FIXME Do we want to put that into OApplication as in KApplication? -zecke
    //      We already have a per-application config in OApplication
    //      ( accessed through oApp->config() ), but this one is the global one! -mickeyl
    static OConfig* config();
    static OConfig* _config;
};

#endif // OGLOBAL_H
