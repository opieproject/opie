#include <stdio.h>
#include <stdlib.h>

#include <qfile.h>
#include <qlayout.h>
#include <qwidgetstack.h>

#include <qpe/config.h>

#include "widget_layer.h"
#include "emulation_widget.h"
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
        prof.setIOLayer( conf.readEntry("iolayer").utf8() );
        prof.setTerminalName( conf.readEntry("term").utf8() );
        qWarning(" %s %s", conf.readEntry("iolayer").latin1(), prof.ioLayerName().data() );
        prof.setBackground( conf.readNumEntry("back") );
        prof.setForeground( conf.readNumEntry("fore") );
        prof.setTerminal( conf.readNumEntry("terminal") );

        // THIS is evil because all data get's reset
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
/*
 * Our goal is to create a Session
 * We will load the the IOLayer and EmulationLayer
 * from the factory
 * we will generate a QWidgetStack
 * add a dummy widget with layout
 * add "Widget" to the layout
 * add the dummy to the stack
 * raise the dummy
 * call session->connect(=
 * this way we only need to reparent
 * in TabWidget
 */
Session* ProfileManager::fromProfile( const Profile& prof,  QWidget* parent) {
    Session* session = new Session();
    session->setName( prof.name() );
    /* translate the internal name to the external */
    session->setIOLayer(m_fact->newIOLayer( m_fact->external(prof.ioLayerName()) ,
                                            prof) );

    QWidgetStack *stack = new QWidgetStack(parent);
    session->setWidgetStack( stack );
    QWidget* dummy = new QWidget( stack );
    QHBoxLayout* lay = new QHBoxLayout(dummy );
    stack->addWidget( dummy, 0 );
    stack->raiseWidget( 0 );
    WidgetLayer* wid = new EmulationWidget( prof, dummy );
    lay->addWidget( wid );

    session->setEmulationWidget( wid );
    session->setEmulationLayer( m_fact->newEmulationLayer( m_fact->external( prof.terminalName() ),
                                                        wid ) );
    session->connect();

    return session;
}
void ProfileManager::save(  ) {
    QFile::remove( (QString(getenv("HOME") )+ "/Settings/opie-console-profiles.conf" ) );
    ProfileConfig conf("opie-console-profiles");
    Profile::ValueList::Iterator it2;
    for (it2 = m_list.begin(); it2 != m_list.end(); ++it2 ) {
        conf.setGroup( (*it2).name() );

        /* now the config stuff */
        QMap<QString, QString> map =  (*it2).conf();
        QMap<QString, QString>::Iterator confIt;
        for ( confIt = map.begin(); confIt != map.end(); ++confIt ) {
            conf.writeEntry( confIt.key(), confIt.data() );
        }

        conf.writeEntry( "name", (*it2).name() );
        QString str = QString::fromUtf8( (*it2).ioLayerName() );
	qWarning("IOLayerName " + str );

        conf.writeEntry( "iolayer", str );
        conf.writeEntry( "term", QString::fromUtf8( (*it2).terminalName()  ) );
        conf.writeEntry( "back", (*it2).background() );
        conf.writeEntry( "fore", (*it2).foreground() );
        conf.writeEntry( "terminal", (*it2).terminal() );
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
