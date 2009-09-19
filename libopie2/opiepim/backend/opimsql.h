/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
              =.             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
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

#ifndef OPIMSQL_H
#define OPIMSQL_H

#include <opie2/opimchangelog_sql.h>
#include <opie2/osqlquery.h>
#include <opie2/opimglobal.h>

namespace Opie {


/**
 * OPimSQLLoadQuery
 * this one queries for all uids
 */
class OPimSQLLoadQuery : public DB::OSQLQuery {
public:
    OPimSQLLoadQuery( const QString &dataTable, OPimChangeLog_SQL *changeLog );
    ~OPimSQLLoadQuery();
    QString query()const;
private:
    QString m_dataTable;
    OPimChangeLog_SQL *m_changeLog;
};

    
/**
 * removes one from the table
 */
class OPimSQLRemoveQuery : public DB::OSQLQuery {
public:
    OPimSQLRemoveQuery( const QString &dataTable, int uid );
    ~OPimSQLRemoveQuery();
    QString query() const;
private:
    QString m_dataTable;
    int m_uid;
};

/**
 * a find query for elements
 */
class OPimSQLFindQuery : public DB::OSQLQuery {
public:
    OPimSQLFindQuery( const QString &dataTable, OPimChangeLog_SQL *changeLog, int uid);
    OPimSQLFindQuery( const QString &dataTable, OPimChangeLog_SQL *changeLog, const UIDArray& );
    ~OPimSQLFindQuery();
    QString query() const;
private:
    QString single() const;
    QString multi() const;
    QString m_dataTable;
    UIDArray m_uids;
    int m_uid;
    OPimChangeLog_SQL *m_changeLog;
};

}

#endif