#ifndef OPIE_PIM_ACCESS_SQL_H
#define OPIE_PIM_ACCESS_SQL_H

#include <qasciidict.h>

#include "otodoaccessbackend.h"

class OSQLDriver;
class OSQLResult;
class OTodoAccessBackendSQL : public OTodoAccessBackend {
public:
    OTodoAccessBackendSQL( const QString& file );
    ~OTodoAccessBackendSQL();

    bool load();
    bool reload();
    bool save();
    QArray<int> allRecords()const;

    QArray<int> queryByExample( const OTodo& t, int sort );
    OTodo find(int uid)const;
    void clear();
    bool add( const OTodo& t );
    bool remove( int uid );
    bool replace( const OTodo& t );

    QArray<int> overDue();
    QArray<int> effectiveToDos( const QDate& start,
                                const QDate& end, bool includeNoDates );
    QArray<int> sorted(bool asc, int sortOrder, int sortFilter, int cat );

private:
    void update();
    void fillDict();
    bool date( QDate& date, const QString& )const;
    OTodo todo( const OSQLResult& )const;
    QArray<int> uids( const OSQLResult& )const;
    OTodo todo( int uid )const;

    QAsciiDict<int> m_dict;
    OSQLDriver* m_driver;
    QArray<int> m_uids;
};


#endif
