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
    enum SortFilter{ Category =1,
                     OnlyOverDue= 2,
                     DoNotShowCompleted =4  };
    /**
     * if you use 0l
     * the default resource will be
     * picked up
     */
    OTodoAccess( OTodoAccessBackend* = 0l, enum Access acc = Random );
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
     * delete all already completed items
     */
    void removeAllCompleted();

    /**
     * request information about what a backend supports.
     * Supports in the sense of beeing able to store.
     * This is related to the enum in OTodo
     *
     * @param backend Will be used in the future when we support multiple backend
     */
    QBitArray backendSupport( const QString& backend = QString::null )const;

    /**
     * see above but for a specefic attribute. This method was added for convience
     * @param attr The attribute to be queried for
     * @param backend Will be used in the future when we support multiple backends
     */
    bool backendSupports( int attr, const QString& backend = QString::null  )const;
signals:
    /**
     * if the OTodoAccess was changed
     */
    void changed( const OTodoAccess* );
    void changed( const OTodoAccess*, int uid );
    void added( const OTodoAccess*,  int uid );
    void removed( const OTodoAccess*, int uid );
private:
    int m_cat;
    OTodoAccessBackend* m_todoBackEnd;
    class OTodoAccessPrivate;
    OTodoAccessPrivate* d;
};

#endif
