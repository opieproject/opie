/*
                             This file is part of the Opie Project

                             Copyright (C) 2003 Patrick S. Vogt <tille@handhelds.org>
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


#include "opieconfig.h"

OpieConfig::OpieConfig( const QString &name, Domain domain )
#ifdef QWS
    :Config( name, domain )
{
#else
    :QSettings(Native), hasGroup(false)
{
    Scope s;
    switch (domain) {
    case File: s = QSettings::Global;
        break;
    case User: s = QSettings::User;
        break;
    }
    setPath("libopie2", name, s );
#endif
}


#ifndef QWS

void OpieConfig::setGroup( const QString& key)
{
    if (hasGroup) endGroup();
    hasGroup = true;
    beginGroup( key );
};

bool OpieConfig::hasKey ( const QString & key ) const
{
    bool *ok;
    readEntry( key, QString::null, ok );
    return *ok;
};
#endif

