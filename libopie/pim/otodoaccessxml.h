#ifndef OPIE_TODO_ACCESS_XML_H
#define OPIE_TODO_ACCESS_XML_H

#include <qasciidict.h>
#include <qmap.h>

#include "otodoaccessbackend.h"

namespace Opie {
    class XMLElement;
};

class OTodoAccessXML : public OTodoAccessBackend {
public:
    /**
     * fileName if Empty we will use the default path
     */
    OTodoAccessXML( const QString& appName,
                    const QString& fileName = QString::null );
    ~OTodoAccessXML();

    bool load();
    bool reload();
    bool save();

    QArray<int> allRecords()const;
    QArray<int> queryByExample( const OTodo&, int querysettings );
    OTodo find( int uid )const;
    void clear();
    bool add( const OTodo& );
    bool remove( int uid );
    bool replace( const OTodo& );

    /* our functions */
    QArray<int> effectiveToDos( const QDate& start,
                                const QDate& end,
                                bool includeNoDates );
    QArray<int> overDue();
    QArray<int> sorted( bool asc,  int sortOrder,
                        int sortFilter, int cat );
private:
    void todo( QAsciiDict<int>*, OTodo&,const QCString&,const QString& );
    QString toString( const OTodo& )const;
    QString toString( const QArray<int>& ints ) const;
    QMap<int, OTodo> m_events;
    QString m_file;
    QString m_app;
    bool m_opened : 1;
    bool m_changed : 1;
    class OTodoAccessXMLPrivate;
    OTodoAccessXMLPrivate* d;
    int m_year, m_month, m_day;

};

#endif
