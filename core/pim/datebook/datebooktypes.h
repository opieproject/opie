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

#ifndef _DATEBOOK_TYPES_H
#define _DATEBOOK_TYPES_H

#include <qpe/datebookdb.h>

#include <qvaluelist.h>
#include <qstringlist.h>

namespace Opie {
namespace Datebook {
    class HolidayPlugin;
    class HolidayPluginIf;
}
namespace Core {
    class OPluginLoader;
    class OPluginManager;
}
}

class QLibrary;

enum { DAY=1,WEEK,WEEKLST,MONTH };  // defaultView values
enum { NONE=0,NORMAL,EXTENDED };    // WeekLstView's modes.

class DateBookDBHack : virtual public DateBookDB {
 public:
  virtual ~DateBookDBHack(){}
  Event eventByUID(int id);
};

class DateBookHoliday
{
public:
    DateBookHoliday();
    virtual ~DateBookHoliday();

    QStringList holidaylist(const QDate&);
    QStringList holidaylist(unsigned year, unsigned month, unsigned day);
    virtual QValueList<EffectiveEvent> getEffectiveEvents(const QDate &from,const QDate &to );
    virtual QValueList<EffectiveEvent> getEffectiveEvents(const QDateTime &start);

    void reloadPlugins();

    Opie::Core::OPluginLoader*pluginLoader(){return m_pluginLoader;}
    Opie::Core::OPluginManager*pluginManager(){return m_pluginManager;}

protected:
    void init();
    void deinit();

    struct HPlugin {
        Opie::Datebook::HolidayPlugin*_plugin;
        //QLibrary*_lib;
        //Opie::Datebook::HolidayPluginIf*_if;
    };
    QValueList<HPlugin*>_pluginlist;
    Opie::Core::OPluginLoader*m_pluginLoader;
    Opie::Core::OPluginManager*m_pluginManager;
};

class DateBookDBHoliday:virtual public DateBookDBHack {
public:
    DateBookDBHoliday():DateBookDBHack(){db_holiday=0;}
    virtual ~DateBookDBHoliday(){}
    virtual QValueList<EffectiveEvent> getEffectiveEvents(const QDate &from,const QDate &to );
    virtual QValueList<EffectiveEvent> getEffectiveEvents(const QDateTime &start);
    virtual QValueList<EffectiveEvent> getEffectiveEventsNoHoliday(const QDate &from,const QDate &to );
    virtual QValueList<EffectiveEvent> getEffectiveEventsNoHoliday(const QDateTime &start);

    DateBookHoliday*db_holiday;
};

#endif
