/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@handhelds.org>
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

#ifndef _HOLIDAY_H
#define _HOLIDAY_H

#include <qvaluelist.h>
#include <qstringlist.h>

#include <opie2/odatebookaccess.h>

namespace Opie {
namespace Datebook {
    class HolidayPlugin2;
    class HolidayPluginIf2;
}
namespace Core {
    class OPluginLoader;
    class OPluginManager;
}
}

class QLibrary;

class DateBookHoliday
{
public:
    DateBookHoliday();
    virtual ~DateBookHoliday();

    QStringList holidaylist(const QDate&);
    QStringList holidaylist(unsigned year, unsigned month, unsigned day);
    virtual Opie::OPimOccurrence::List getEffectiveEvents(const QDate &from,const QDate &to );
    virtual Opie::OPimOccurrence::List getEffectiveEvents(const QDateTime &start);

    void reloadPlugins();

    Opie::Core::OPluginLoader*pluginLoader(){return m_pluginLoader;}
    Opie::Core::OPluginManager*pluginManager(){return m_pluginManager;}

protected:
    void init();
    void deinit();

    struct HPlugin {
        Opie::Datebook::HolidayPlugin2*_plugin;
        //QLibrary*_lib;
        //Opie::Datebook::HolidayPluginIf2*_if;
        Opie::ODateBookAccess *_access;
    };
    QValueList<HPlugin*>_pluginlist;
    Opie::Core::OPluginLoader*m_pluginLoader;
    Opie::Core::OPluginManager*m_pluginManager;
};

#endif
