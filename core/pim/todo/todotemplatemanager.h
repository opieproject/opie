#ifndef OPIE_TODO_TEMPLATE_H
#define OPIE_TODO_TEMPLATE_H

#include <qstring.h>
#include <qmap.h>

#include <opie/todoevent.h>

using namespace Opie;

namespace Todo {
    class TemplateManager {
    public:
        TemplateManager();
        ~TemplateManager();
        void load();
        void save();
        QStringList templates()const;
        ToDoEvent templateEvent( const QString& templateName );

        /* also replaces one */
        void addEvent( const QString&, const ToDoEvent& );
        void removeEvent( const QString& );
    private:
        QString m_path;
        QMap<QString, ToDoEvent> m_templates;


    };
};

#endif
