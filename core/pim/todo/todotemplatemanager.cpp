#include <qpe/config.h>
#include <qpe/global.h>

#include <opie/tododb.h>
#include <opie/todoxmlresource.h>

#include "todotemplatemanager.h"


using namespace Todo;

TemplateManager::TemplateManager() {
    m_path =  Global::applicationFileName("todolist", "templates.xml");
}
TemplateManager::~TemplateManager() {

}
void TemplateManager::load() {
    Config conf("todolist_templates");
    ToDoXMLResource *xml = new ToDoXMLResource( QString::fromLatin1("template"),
                                                m_path );
    ToDoDB todoDB(xml );
    todoDB.load();

    ToDoDB::Iterator it = todoDB.rawToDos();
    for ( ; it != todoDB.end(); ++it ) {
        ToDoEvent ev = (*it);
        conf.setGroup( QString::number( ev.uid() ) );
        m_templates.insert( conf.readEntry("name", QString::null),
                            ev );
    }
}
void TemplateManager::save() {
    Config conf("todolist_templates");

    ToDoXMLResource *res = new ToDoXMLResource( "template",
                                                m_path );
    ToDoDB db(res);


    QMap<QString, ToDoEvent>::Iterator it;
    for ( it = m_templates.begin(); it != m_templates.end(); ++it ) {
        ToDoEvent ev = (*it);
        conf.setGroup( QString::number( ev.uid() ) );
        conf.writeEntry("Name", it.key() );
        db.addEvent( ev );
    }
    db.save();
}
void TemplateManager::addEvent( const QString& str,
                                const ToDoEvent& ev) {
    m_templates.replace( str,  ev );
}
void TemplateManager::removeEvent( const QString& str ) {
    m_templates.remove( str );
}
QStringList TemplateManager::templates() const {
    QStringList list;
    QMap<QString, ToDoEvent>::ConstIterator it;
    for (it = m_templates.begin(); it != m_templates.end(); ++it ) {
        list << it.key();
    }

    return list;
}
ToDoEvent TemplateManager::templateEvent( const QString& templateName ) {
    return m_templates[templateName];
}
