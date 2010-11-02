/*
                             This file is part of the Opie Project
                             Copyright (C) The Main Author <main-author@whereever.org>
              =.             Copyright (C) The Opie Team <opie-devel@lists.sourceforge.net>
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
#ifndef OPIE_TODO_ACCESS_BACKEND_H
#define OPIE_TODO_ACCESS_BACKEND_H

#include <qbitarray.h>

#include <opie2/opimtodo.h>
#include <opie2/opimaccessbackend.h>

namespace Opie {
class OPimTodoAccessBackend : public OPimAccessBackend<OPimTodo> {
public:
    OPimTodoAccessBackend();
    ~OPimTodoAccessBackend();
    virtual UIDArray effectiveToDos( const QDate& start,
                                     const QDate& end,
                                     bool includeNoDates )const = 0;
    virtual UIDArray overDue()const = 0;
    virtual void removeAllCompleted() = 0;

    /**
     * Common and probably inefficent implementation
     * for queryByExample, matchRegexp, sorted
     * and occurrences
     */
    //@{
    /**
     *  Return all possible settings.
     *  @return All settings provided by the current backend
     * (i.e.: query_WildCards & query_IgnoreCase)
     */
    uint querySettings() const;

    /**
     * Check whether settings are correct.
     * @return <i>true</i> if the given settings are correct and possible.
     */
    bool hasQuerySettings (uint querySettings) const;

    UIDArray queryByExample( const UIDArray& uidlist, const OPimTodo& query, int settings, const QDateTime& startperiod = QDateTime() )const;
    UIDArray sorted( const UIDArray&, bool asc, int, int, const QArray<int>& )const;
    OPimBackendOccurrence::List occurrences( const QDate&, const QDate& )const;
    //@}

private:
    class Private;
    Private *d;

};
}


/**
 * \fn Opie::OPimBackendOccurrence::List Opie::OPimTodoAccessBackend::occurrences(const QDate& start,const QDate& end)const
 * \brief Return occurrences for a period of time
 *
 * This method will return the 'effective' Todos and also
 * 'Overdue' Todos. Overdues will be shown on the 'current'
 * day if it is in the range or on \par start. If the overdue
 * is inside the 'Effective Todos' we will skip the
 * special overdue handling.
 *
 *
 */
#endif
