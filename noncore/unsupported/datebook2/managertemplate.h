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
        typedef typename QMap<QString, T>::ConstIterator Iterator;
    public:
        ManagerTemplate();
        virtual ~ManagerTemplate();

        virtual void add( const QString&, const T& t );
        void remove( const QString& );
        virtual bool load() = 0;
        virtual bool save() = 0;

        QStringList names()const;
        T value(const QString&)const;

    protected:
        QMap<QString, T> m_map;
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
    QStringList ManagerTemplate<T>::names()const {
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
