#ifndef OPIE_TODO_TEMPLATE_H
#define OPIE_TODO_TEMPLATE_H

#include <opie2/opimtodo.h>

#include <qstring.h>
#include <qmap.h>

using namespace Opie;

namespace Todo {
    class TemplateManager {
    public:
        TemplateManager();
        ~TemplateManager();
        void load();
        void save();
        QStringList templates()const;
        OPimTodo templateEvent( const QString& templateName );

        /* also replaces one */
        void addEvent( const QString&, const OPimTodo& );
        void removeEvent( const QString& );
    private:
        QString m_path;
        QMap<QString, OPimTodo> m_templates;



    };
};

#endif
