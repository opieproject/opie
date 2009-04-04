/*
                             This file is part of the Opie Project
                             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
              =.             
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include <qvaluelist.h>

#include <opie2/odebug.h>
#include <opie2/opluginloader.h>
#include <opie2/oholidaypluginif2.h>
#include <opie2/oholidayplugin2.h>
#include <opie2/opimoccurrence.h>

#include <opie2/odatebookaccess.h>
#include "odatebookaccessbackend_holiday.h"

#include "holiday.h"

using namespace Opie;
using namespace Opie::Datebook;

DateBookHoliday::DateBookHoliday()
{
    _pluginlist.clear();
    m_pluginLoader = new Opie::Core::OPluginLoader("holidays2",false);
    m_pluginLoader->setAutoDelete(true);
    m_pluginManager = new Opie::Core::OPluginManager(m_pluginLoader);
    m_pluginManager->load();

    init();
}

void DateBookHoliday::reloadPlugins()
{
    deinit();
    init();
}

DateBookHoliday::~DateBookHoliday()
{
    deinit();
    delete m_pluginLoader;
    delete m_pluginManager;
}

void DateBookHoliday::deinit()
{
    QValueList<HPlugin*>::Iterator it;
    for (it=_pluginlist.begin();it!=_pluginlist.end();++it) {
        HPlugin*_pl = *it;
        *it = 0;
        delete _pl;
    }
    _pluginlist.clear();
}

#if 0
void debugLst( const Opie::Core::OPluginItem::List& lst ) {
    for ( Opie::Core::OPluginItem::List::ConstIterator it = lst.begin(); it != lst.end(); ++it )
        odebug << "Name " << (*it).name() << " " << (*it).path() << " " << (*it).position() << oendl;
}
#endif

void DateBookHoliday::init()
{
    deinit();

    Opie::Core::OPluginItem::List  lst = m_pluginLoader->filtered(true);
//    debugLst( lst );
    for( Opie::Core::OPluginItem::List::Iterator it = lst.begin(); it != lst.end(); ++it ){
        Opie::Datebook::HolidayPluginIf2*hif = m_pluginLoader->load<Opie::Datebook::HolidayPluginIf2>(*it,IID_HOLIDAY_PLUGIN);
        if (hif) {
            Opie::Datebook::HolidayPlugin2 *pl = hif->plugin();
            if (pl) {
                HPlugin*_pl=new HPlugin;
                _pl->_plugin = pl;
                odebug << "Found holiday " << pl->description() << oendl;
                ODateBookAccessBackend_Holiday *backend = new ODateBookAccessBackend_Holiday(_pl->_plugin);
                _pl->_access = new ODateBookAccess(backend);
                _pluginlist.append(_pl);
                //_pl->_if = hif;
            }
        }
    }
}

QStringList DateBookHoliday::holidaylist(const QDate&aDate)
{
    QStringList ret;
    QValueList<HPlugin*>::Iterator it;
    for (it=_pluginlist.begin();it!=_pluginlist.end();++it) {
        HPlugin*_pl = *it;
        ret+=_pl->_plugin->entries(aDate);
    }
    return ret;
}

QStringList DateBookHoliday::holidaylist(unsigned year, unsigned month, unsigned day)
{
    return holidaylist(QDate(year,month,day));
}

OPimOccurrence::List DateBookHoliday::getEffectiveEvents(const QDate &from,const QDate &to )
{
    OPimOccurrence::List ret;
    QValueList<HPlugin*>::Iterator it;
    for (it=_pluginlist.begin();it!=_pluginlist.end();++it) {
        HPlugin*_pl = *it;
        ret += _pl->_access->effectiveNonRepeatingEvents(from,to);
    }
    return ret;
}

OPimOccurrence::List DateBookHoliday::getEffectiveEvents(const QDateTime &d)
{
    return getEffectiveEvents( d.date(), d.date() );
}
