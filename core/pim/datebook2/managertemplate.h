#ifndef OPIE_DATE_BOOK_MANAGER_TEMPLATE_H
#define OPIE_DATE_BOOK_MANAGER_TEMPLATE_H

#include <qdialog.h>
#include <qmap.h>
#include <qstring.h>

namespace Datebook {
    /**
     * ManagerTemplate is a template which manages
     * all kind of managers :)
     */
    template<class T>
    class ManagerTemplate {
        typedef typename QMap<QString, T>::Iterator Iterator;
    public:
        ManagerTemplate();
        virtual ~ManagerTemplate();

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
    template<class T>
    ManagerTemplate<T>::ManagerTemplate() {
    }
    template<class T>
    ManagerTemplate<T>::~ManagerTemplate() {
    }
    template<class T>
    void ManagerTemplate<T>::add( const QString& str, const T& t ) {
        m_map.insert( str, t );
    }
    template<class T>
    void ManagerTemplate<T>::remove( const QString& str ) {
        m_map.remove( str );
    }
    template<class T>
    bool ManagerTemplate<T>::load() {
        return doLoad();
    }
    template<class T>
    bool ManagerTemplate<T>::save() {
        return doSave();
    }
    template<class T>
    QStringList ManagerTemplate<T>::names() {
        QStringList lst;
        Iterator it;
        for ( it = m_map.begin(); it != m_map.end(); ++it ) {
            lst << it.key();
        }
        return lst;
    }
    template<class T>
    T ManagerTemplate<T>::value( const QString& str)const {
        return m_map[str];
    }
}

#endif
