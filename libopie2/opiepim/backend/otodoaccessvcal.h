#ifndef OPIE_OTODO_ACCESS_VCAL_H
#define OPIE_OTODO_ACCESS_VCAL_H

#include "otodoaccessbackend.h"

class OTodoAccessVCal : public OTodoAccessBackend {
public:
    OTodoAccessVCal(const QString& );
    ~OTodoAccessVCal();

    bool load();
    bool reload();
    bool save();

    QArray<int> allRecords()const;
    QArray<int> queryByExample( const OTodo& t, int sort );
    QArray<int> effectiveToDos( const QDate& start,
                                const QDate& end,
                                bool includeNoDates );
    QArray<int> overDue();
    QArray<int> sorted( bool asc, int sortOrder, int sortFilter,
                        int cat );
    OTodo find(int uid)const;
    void clear();
    bool add( const OTodo& );
    bool remove( int uid );
    bool replace( const OTodo& );

private:
    bool m_dirty : 1;
    QString m_file;
    QMap<int, OTodo> m_map;
};

#endif
