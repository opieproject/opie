#ifndef OPIE_TODO_TEMPLATE_H
#define OPIE_TODO_TEMPLATE_H

#include <qstring.h>
#include <qmap.h>

#include <opie/otodo.h>


namespace Todo {
    class TemplateManager {
    public:
        TemplateManager();
        ~TemplateManager();
        void load();
        void save();
        QStringList templates()const;
        OTodo templateEvent( const QString& templateName );

        /* also replaces one */
        void addEvent( const QString&, const OTodo& );
        void removeEvent( const QString& );
    private:
        QString m_path;
        QMap<QString, OTodo> m_templates;



    };
};

#endif
