/*
                             This file is part of the Opie Project
                             Copyright (C) The Main Author <main-author@whereever.org>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/
#ifndef OPIE_TODO_ACCESS_H
#define OPIE_TODO_ACCESS_H

#include <qobject.h>
#include <qvaluelist.h>

#include <opie2/opimtodo.h>
#include <opie2/otodoaccessbackend.h>
#include <opie2/opimaccesstemplate.h>

namespace Opie {

/**
 * OPimTodoAccess
 * the class to get access to
 * the todolist
 */
class OPimTodoAccess : public QObject, public OPimAccessTemplate<OPimTodo> {
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
    OPimTodoAccess( OPimTodoAccessBackend* = 0l, enum Access acc = Random );
    ~OPimTodoAccess();


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
     * return overdue OPimTodos
     */
    List overDue();

    /**
     *
     */
    List sorted( bool ascending, int sortOrder, int sortFilter, int cat );

    /**
     * merge a list of OPimTodos into
     * the resource
     */
    void mergeWith( const QValueList<OPimTodo>& );

    /**
     * delete all already completed items
     */
    void removeAllCompleted();

    /**
     * request information about what a backend supports.
     * Supports in the sense of beeing able to store.
     * This is related to the enum in OPimTodo
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
     * if the OPimTodoAccess was changed
     */
    void changed( const OPimTodoAccess* );
    void changed( const OPimTodoAccess*, int uid );
    void added( const OPimTodoAccess*,  int uid );
    void removed( const OPimTodoAccess*, int uid );
private:
    int m_cat;
    OPimTodoAccessBackend* m_todoBackEnd;
    class OPimTodoAccessPrivate;
    OPimTodoAccessPrivate* d;
};

}

#endif
