#ifndef OPIE_TODO_ACCESS_H
#define OPIE_TODO_ACCESS_H

#include <qobject.h>
#include <qvaluelist.h>

#include "otodo.h"
#include "otodoaccessbackend.h"
#include "opimaccesstemplate.h"


/**
 * OTodoAccess
 * the class to get access to
 * the todolist
 */
class OTodoAccess : public QObject, public OPimAccessTemplate<OTodo> {
    Q_OBJECT
public:
    enum SortOrder { Completed = 0,
                     Priority,
                     Description,
                     Deadline };
    enum SortFilter{ ShowOverdue = 0,
                     Category =1,
                     OnlyOverDue= 2 };
    /**
     * if you use 0l
     * the default resource will be
     * picked up
     */
    OTodoAccess( OTodoAccessBackend* = 0l);
    ~OTodoAccess();


    /* our functions here */
    /**
     * include todos from start to end
     * includeNoDates whether or not to include
     * events with no dates
     */
    List effectiveToDos( const QDate& start,
                         const QDate& end,
                         bool includeNoDates = true );

    /**
     * start
     * end date taken from the currentDate()
     */
    List effectiveToDos( const QDate& start,
                         bool includeNoDates = true );


    /**
     * return overdue OTodos
     */
    List overDue();

    /**
     *
     */
    List sorted( bool ascending, int sortOrder, int sortFilter, int cat );

    /**
     * merge a list of OTodos into
     * the resource
     */
    void mergeWith( const QValueList<OTodo>& );

    /**
     * add an Alarm to the AlarmServer
     */
    void addAlarm( const OTodo& );

    /**
     * delete an alarm with the uid from
     * the alarm server
     */
    void delAlarm( int uid );

signals:
    /**
     * if the OTodoAccess was changed
     */
    void signalChanged( const OTodoAccess* );
private:
    int m_cat;
    OTodoAccessBackend* m_todoBackEnd;
    class OTodoAccessPrivate;
    OTodoAccessPrivate* d;
};

#endif
