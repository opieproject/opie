#include <opie2/odebug.h>
#include <opie2/otodoaccess.h>
#include <opie2/otodoaccessxml.h>

#include <qpe/config.h>
#include <qpe/global.h>

#include "todotemplatemanager.h"


using namespace Todo;

TemplateManager::TemplateManager() {
    m_path =  Global::applicationFileName("todolist", "templates.xml");
}
TemplateManager::~TemplateManager() {
    save();
}
void TemplateManager::load() {
    Config conf("todolist_templates");
    OPimTodoAccessXML *xml = new OPimTodoAccessXML( QString::fromLatin1("template"),
                                                m_path );
    OPimTodoAccess todoDB(xml );
    todoDB.load();

    OPimTodoAccess::List::Iterator it;
    OPimTodoAccess::List list = todoDB.allRecords();
    for ( it = list.begin(); it != list.end(); ++it ) {
        OPimTodo ev = (*it);
        conf.setGroup( QString::number( ev.uid() ) );
        QString str = conf.readEntry("Name", QString::null );
        if (str.isEmpty() )
            continue;

        m_templates.insert( str, ev );
    }
}
void TemplateManager::save() {
    Opie::Core::owarn << "Saving!!!!" << oendl;
    Config conf("todolist_templates");

    OPimTodoAccessXML *res = new OPimTodoAccessXML( "template",
                                                m_path );
    OPimTodoAccess db(res);
    db.load();
    db.clear();


    QMap<QString, OPimTodo>::Iterator it;
    for ( it = m_templates.begin(); it != m_templates.end(); ++it ) {
        OPimTodo ev = it.data();
        conf.setGroup( QString::number( ev.uid() ) );
        Opie::Core::owarn << "Name " << it.key() << oendl;
        conf.writeEntry("Name", it.key() );
        db.add( ev );
    }
    db.save();
}
void TemplateManager::addEvent( const QString& str,
                                const OPimTodo& ev) {
    Opie::Core::owarn << "AddEvent " << str << oendl;
    OPimTodo todo = ev;
    if( ev.uid() == 0 )
	todo.setUid(1); // generate a new uid

    m_templates.replace( str,  todo );
}
void TemplateManager::removeEvent( const QString& str ) {
    m_templates.remove( str );
}
QStringList TemplateManager::templates() const {
    QStringList list;
    QMap<QString, OPimTodo>::ConstIterator it;
    for (it = m_templates.begin(); it != m_templates.end(); ++it ) {
        list << it.key();
    }

    return list;
}
OPimTodo TemplateManager::templateEvent( const QString& templateName ) {
    return m_templates[templateName];
}
