#ifndef OPIE_DATE_BOOK_MANAGER_TEMPLATE_H
#define OPIE_DATE_BOOK_MANAGER_TEMPLATE_H

#include <qdialog.h>
#include <qstring.h>

namespace Datebook {
    /**
     * ManagerTemplate is a template which manages
     * all kind of managers :)
     */
    template<class T>
    class ManagerTemplate {
    public:
        ManagerTemplate();
        ~ManagerTemplate();

        virtual void add( const QString&, const T& t );
        void remove( const QString& );
        bool load();
        bool save();

        QStringList names();
        T value(const QString&)const;

    protected:
        QMap<QString, T> m_map;

    private:
        virtual bool doSave() = 0;
        virtual bool doLoad() = 0;

    };
}

#endif
