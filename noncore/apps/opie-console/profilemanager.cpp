
#include <qpe/config.h>

#include "metafactory.h"
#include "profileconfig.h"
#include "profilemanager.h"

ProfileManager::ProfileManager( MetaFactory* fact )
    : m_fact( fact )
{

}
ProfileManager::~ProfileManager() {

}
void ProfileManager::load() {
    m_list.clear();
    qWarning("load");
    ProfileConfig conf("opie-console-profiles");
    QStringList groups = conf.groups();
    QStringList::Iterator it;

    /*
     * for each profile
     */
    for ( it = groups.begin(); it != groups.end(); ++it ) {
        qWarning("group " + (*it) );
        conf.setGroup( (*it) );
        Profile prof;
        prof.setName( conf.readEntry("name") );
        prof.setIOLayer( conf.readEntry("iolayer") );
        prof.setBackground( conf.readNumEntry("back") );
        prof.setForeground( conf.readNumEntry("fore") );
        prof.setTerminal( conf.readNumEntry("terminal") );
        prof.setConf( conf.items( (*it) ) );

        /* now add it */
        m_list.append( prof );
    }

}
void ProfileManager::clear() {
    m_list.clear();
}
Profile::ValueList ProfileManager::all()const {
    return m_list;
}
Session* ProfileManager::fromProfile( const Profile& prof) {
    Session* session = new Session();
    session->setName( prof.name() );
    session->setIOLayer(m_fact->newIOLayer(prof.ioLayerName(),
                                           prof) );
    /*
     * FIXME
     * load emulation
     * load widget?
     * set colors + fonts
     */
    return session;
}
void ProfileManager::save(  ) {
    ProfileConfig conf("opie-console-profiles");
    conf.clearAll();
    Profile::ValueList::Iterator it;
    for (it = m_list.begin(); it != m_list.end(); ++it ) {
        conf.setGroup( (*it).name() );
        conf.writeEntry( "name", (*it).name() );
        conf.writeEntry( "ioplayer", (*it).ioLayerName() );
        conf.writeEntry( "back", (*it).background() );
        conf.writeEntry( "fore", (*it).foreground() );
        conf.writeEntry( "terminal", (*it).terminal() );
        /* now the config stuff */
        QMap<QString, QString> map =  (*it).conf();
        QMap<QString, QString>::Iterator it;
        for ( it = map.begin(); it != map.end(); ++it ) {
            conf.writeEntry( it.key(), it.data() );
        }
    }
}
void ProfileManager::setProfiles( const Profile::ValueList& list ) {
    m_list = list;
};
Profile ProfileManager::profile( const QString& name )const {
    Profile prof;
    Profile::ValueList::ConstIterator it;
    for ( it = m_list.begin(); it != m_list.end(); ++it ) {
        if ( name == (*it).name() ) {
            prof = (*it);
            break;
        }
    }
    return prof;
}
