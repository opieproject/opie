/*
                             This file is part of the OPIE Project
                             
               =.            Copyright (c)  2002 Andy Qua <andy.qua@blueyonder.co.uk>
             .=l.                                Dan Williams <drw@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef DESTINATION_H
#define DESTINATION_H

#include <qstring.h>

class Destination
{
public:
    Destination() {}
    Destination( const char *name, const char *path );
    ~Destination();

    void setDestinationName( const QString &name ) { destName = name; }
    void setDestinationPath( const QString &path )   { destPath = path; }
    void linkToRoot( bool val )             { linkToRootDir = val; }

    bool linkToRoot()             { return linkToRootDir; }
    QString &getDestinationName() { return destName; }
    QString &getDestinationPath()  { return destPath; }

protected:

private:
  QString destName;
  QString destPath;
  bool linkToRootDir;
};

#endif
