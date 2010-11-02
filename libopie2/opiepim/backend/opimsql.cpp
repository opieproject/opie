/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
              =.             Copyright (C) 2009 The Opie Team <opie-devel@lists.sourceforge.net>
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

#include <opie2/odebug.h>

#include "opimsql.h"

using namespace Opie;
using namespace Opie::DB;


OPimSQLLoadQuery::OPimSQLLoadQuery( const QString &dataTable, OPimChangeLog_SQL *changeLog )
    : OSQLQuery(), m_dataTable( dataTable ), m_changeLog( changeLog )
{
}

OPimSQLLoadQuery::~OPimSQLLoadQuery()
{
}

QString OPimSQLLoadQuery::query() const
{
    QString qu;
    bool slowSync = m_changeLog->slowSync();
    if( slowSync ) {
        qu = "select uid from " + m_dataTable;
    }
    else {
        int logId = m_changeLog->peerLastSyncLogId();
        qu = "SELECT uid FROM changelog";
        qu += " WHERE logid > " + QString::number( logId );
    }

    odebug << " ******* query = " << qu << oendl;

    return qu;
}

//---------------------------------------------------------------------------

OPimSQLRemoveQuery::OPimSQLRemoveQuery( const QString &dataTable, int uid )
    : OSQLQuery(), m_dataTable( dataTable ), m_uid( uid )
{
}

OPimSQLRemoveQuery::~OPimSQLRemoveQuery()
{
}

QString OPimSQLRemoveQuery::query() const
{
    QString qu = "DELETE from " + m_dataTable + " where uid = "
        + QString::number(m_uid) + ";";

    odebug << qu << oendl;
    return qu;
}

//---------------------------------------------------------------------------

OPimSQLFindQuery::OPimSQLFindQuery( const QString &dataTable, OPimChangeLog_SQL *changeLog, int uid )
    : OSQLQuery(), m_dataTable( dataTable ), m_uid( uid ), m_changeLog( changeLog )
{
}

OPimSQLFindQuery::OPimSQLFindQuery( const QString &dataTable, OPimChangeLog_SQL *changeLog, const UIDArray& ints )
    : OSQLQuery(), m_dataTable( dataTable ), m_uids( ints ), m_uid( 0 ), m_changeLog( changeLog )
{
}

OPimSQLFindQuery::~OPimSQLFindQuery()
{
}

QString OPimSQLFindQuery::query() const
{
    if ( m_uids.count() == 0 )
        return single();
    else
        return multi();
}

QString OPimSQLFindQuery::multi() const
{
    QString qu;

    if( !m_changeLog->slowSync() ) {
        // If there was an easy way we would be better off excluding
        // the uid from the data table here
        int logId = m_changeLog->peerLastSyncLogId();
        qu = "SELECT " + m_dataTable + ".*, changelog.uid as uid, changelog.chgtype";
        qu += " FROM changelog ";
        qu += " LEFT OUTER JOIN " + m_dataTable + " ON " + m_dataTable + ".uid = changelog.uid";
        qu += " WHERE changelog.logid > " + QString::number( logId );
        qu += " AND changelog.uid IN (";
    }
    else
        qu = "SELECT * FROM " + m_dataTable + " WHERE uid IN (";

    for (uint i = 0; i < m_uids.count(); i++ ) {
        qu += QString::number( m_uids[i] ) + ",";
    }
    qu.remove( qu.length()-1, 1 );
    qu += ")";

    odebug << "find query: " << qu << "" << oendl;
    return qu;
}

QString OPimSQLFindQuery::single() const
{
    QString qu;
    if( !m_changeLog->slowSync() ) {
        int logId = m_changeLog->peerLastSyncLogId();
        qu = "SELECT " + m_dataTable + ".*, changelog.uid as uid, changelog.chgtype";
        qu += " FROM changelog ";
        qu += " LEFT OUTER JOIN " + m_dataTable + " ON " + m_dataTable + ".uid = changelog.uid";
        qu += " WHERE changelog.logid > " + QString::number( logId );
        qu += " AND changelog.uid = " + QString::number( m_uid );
    }
    else
        qu = "SELECT * FROM " + m_dataTable + " WHERE uid = " + QString::number(m_uid);

    // owarn << "find query: " << qu << "" << oendl;
    return qu;
}
