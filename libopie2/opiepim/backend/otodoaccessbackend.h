#ifndef OPIE_TODO_ACCESS_BACKEND_H
#define OPIE_TODO_ACCESS_BACKEND_H

#include "otodo.h"
#include "opimaccessbackend.h"

class OTodoAccessBackend : public OPimAccessBackend<OTodo> {
public:
    OTodoAccessBackend();
    ~OTodoAccessBackend();
    virtual QArray<int> effectiveToDos( const QDate& start,
                                        const QDate& end,
                                        bool includeNoDates ) = 0;
    virtual QArray<int> overDue() = 0;

};

#endif
