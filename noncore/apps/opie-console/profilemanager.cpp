
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
    ProfileConfig conf("opie-console-profiles");
    QStringList groups = conf.groups();
    QStringList::Iterator it;

    /*
     * for each profile
     */
    /*
     * QAsciiDict Parsing FIXME
     */
    for ( it = groups.begin(); it != groups.end(); ++it ) {
        conf.setGroup( (*it) );
        Profile prof;
        prof.setName( conf.readEntry("name") );
        prof.setIOLayer( conf.readEntry("iolayer") );
        prof.setBackground( conf.readNumEntry("back") );
        prof.setForeground( conf.readNumEntry("fore") );
        prof.setTerminal( conf.readNumEntry("terminal") );

        m_list.append( prof );
    }

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
     * load widget
     * set colors + fonts
     */
    return session;
}
void ProfileManager::save(  ) {
    m_list.clear();
    ProfileConfig conf("opie-console-profiles");
    Session* se= 0l;
    // FIXME save
}
