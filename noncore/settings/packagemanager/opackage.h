/*
                            This file is part of the Opie Project

                             Copyright (c)  2003 Dan Williams <drw@handhelds.org>
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

#ifndef OPACKAGE_H
#define OPACKAGE_H

#include <qlist.h>
#include <qstring.h>

class OPackage
{
public:
    enum Command { Install, Remove, Update, Upgrade, Download, Info, Files, Version, NotDefined };

    OPackage( const QString &name = 0x0,
              const QString &version = 0x0, const QString &versionInstalled = 0x0,
              const QString &source = 0x0, const QString &destination = 0x0,
              const QString &category = "misc" );

    const QString &name()             { return m_name; }
    const QString &version()          { return m_version; }
    const QString &versionInstalled() { return m_versionInstalled; }
    const QString &source()           { return m_source; }
    const QString &destination()      { return m_destination; }
    const QString &category()         { return m_category; }

    void setName( const QString &name )                { m_name = name; }
    void setVersion( const QString &version )          { m_version = version; }
    void setVersionInstalled( const QString &version ) { m_versionInstalled = version; }
    void setSource( const QString &source )            { m_source = source; }
    void setDestination( const QString &destination )  { m_destination = destination; }
    void setCategory( const QString &category )        { m_category = category; }

private:
    QString m_name;             // Name of item
    QString m_version;          // Available version number of item
    QString m_versionInstalled; // Installed version number of item, null if not installed
    QString m_source;           // Source feed of available version
    QString m_destination;      // Location item is installed to, null if not installed
    QString m_category;         // Item category
};

typedef QList<OPackage>         OPackageList;
typedef QListIterator<OPackage> OPackageListIterator;

#endif
