#include <qpe/config.h>
#include <qpe/global.h>

#include <opie/otodoaccess.h>
#include <opie/otodoaccessxml.h>

#include "todotemplatemanager.h"


using namespace Todo;

TemplateManager::TemplateManager() {
    m_path =  Global::applicationFileName("todolist", "templates.xml");
}
TemplateManager::~TemplateManager() {

}
void TemplateManager::load() {
    Config conf("todolist_templates");
    OTodoAccessXML *xml = new OTodoAccessXML( QString::fromLatin1("template"),
                                                m_path );
    OTodoAccess todoDB(xml );
    todoDB.load();

    OTodoAccess::List::Iterator it;
    OTodoAccess::List list = todoDB.allRecords();
    for ( it = list.begin(); it != list.end(); ++it ) {
        OTodo ev = (*it);
        conf.setGroup( QString::number( ev.uid() ) );
        QString str = conf.readEntry("Name", QString::null );
        if (str.isEmpty() )
            continue;

        m_templates.insert( str,
                            ev );
    }
}
void TemplateManager::save() {
    Config conf("todolist_templates");

    OTodoAccessXML *res = new OTodoAccessXML( "template",
                                                m_path );
    OTodoAccess db(res);


    QMap<QString, OTodo>::Iterator it;
    for ( it = m_templates.begin(); it != m_templates.end(); ++it ) {
        OTodo ev = it.data();
        conf.setGroup( QString::number( ev.uid() ) );
        qWarning("Name" + it.key() );
        conf.writeEntry("Name", it.key() );
        db.add( ev );
    }
    db.save();
}
void TemplateManager::addEvent( const QString& str,
                                const OTodo& ev) {
    qWarning("AddEvent"+  str );
    m_templates.replace( str,  ev );
}
void TemplateManager::removeEvent( const QString& str ) {
    m_templates.remove( str );
}
QStringList TemplateManager::templates() const {
    QStringList list;
    QMap<QString, OTodo>::ConstIterator it;
    for (it = m_templates.begin(); it != m_templates.end(); ++it ) {
        list << it.key();
    }

    return list;
}
OTodo TemplateManager::templateEvent( const QString& templateName ) {
    return m_templates[templateName];
}
