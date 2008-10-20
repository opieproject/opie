#include <qvaluelist.h>

#include <opie2/odebug.h>
#include <opie2/opluginloader.h>
#include <opie2/oholidaypluginif.h>
#include <opie2/oholidayplugin.h>

#include "datebooktypes.h"

using namespace Opie;
using namespace Opie::Datebook;

DateBookHoliday::DateBookHoliday()
{
    _pluginlist.clear();
    m_pluginLoader = new Opie::Core::OPluginLoader("holidays",false);
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
        Opie::Datebook::HolidayPluginIf*hif = m_pluginLoader->load<Opie::Datebook::HolidayPluginIf>(*it,IID_HOLIDAY_PLUGIN);
        if (hif) {
            Opie::Datebook::HolidayPlugin*pl = hif->plugin();
            if (pl) {
                HPlugin*_pl=new HPlugin;
                _pl->_plugin = pl;
                odebug << "Found holiday " << pl->description()<<oendl;
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

QValueList<EffectiveEvent> DateBookHoliday::getEffectiveEvents(const QDate &from,const QDate &to )
{
    QValueList<EffectiveEvent> ret;
    QValueList<HPlugin*>::Iterator it;
    for (it=_pluginlist.begin();it!=_pluginlist.end();++it) {
        HPlugin*_pl = *it;
        ret+=_pl->_plugin->events(from,to);
    }
    return ret;
}

QValueList<EffectiveEvent> DateBookDBHoliday::getEffectiveEventsNoHoliday(const QDate &from,const QDate &to )
{
    return DateBookDBHack::getEffectiveEvents(from,to);
}

QValueList<EffectiveEvent> DateBookDBHoliday::getEffectiveEventsNoHoliday(const QDateTime &start)
{
    return DateBookDBHack::getEffectiveEvents(start);
}

QValueList<EffectiveEvent> DateBookHoliday::getEffectiveEvents(const QDateTime &start)
{
    return getEffectiveEvents(start.date(),start.date());
}

QValueList<EffectiveEvent> DateBookDBHoliday::getEffectiveEvents(const QDate &from,const QDate &to )
{
    QValueList<EffectiveEvent> ret;
    odebug << "Ueberlagert 1" << oendl;
    if (db_holiday) {
        ret+=db_holiday->getEffectiveEvents(from,to);
    }
    ret+=getEffectiveEventsNoHoliday(from,to);
    return ret;
}

QValueList<EffectiveEvent> DateBookDBHoliday::getEffectiveEvents( const QDateTime &start)
{
    odebug << "Ueberlagert 2" << oendl;
    return DateBookDBHack::getEffectiveEvents(start);
}
