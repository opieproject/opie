/*
                             This file is part of the Opie Project
                             Copyright (C) 2009 Paul Eggleton <bluelightning@bluelightning.org>
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

#ifndef OPIMCHANGELOG_SQL_H
#define OPIMCHANGELOG_SQL_H

#include <qstring.h>
#include <qlist.h>

#include <opie2/opimchangelog.h>

namespace Opie {
namespace DB {
class OSQLDriver;
}
}

namespace Opie
{

class OPimChangeLog_SQL : public OPimChangeLog
{
public:
    OPimChangeLog_SQL( Opie::DB::OSQLDriver* driver, const QString &logTable, const QString &peerTable );
    void setEnabled( bool enabled );
    uint peerLastSyncLogId();
    bool slowSync();
    
    virtual void init();
    virtual void addAddEntry( int uid );
    virtual void addUpdateEntry( int uid );
    virtual void addDeleteEntry( int uid );
    virtual void purge();
    virtual QList<OPimSyncPeer> peerList();
    virtual void removePeer( const QString &peerId );
    virtual bool startSync( const OPimSyncPeer &peer, bool slowSync );
    virtual void syncDone();

private:
    void purgeOldData();
    bool entryExists( int uid, bool addedonly );
    bool peersSynced();
    
    OPimSyncPeer m_peer;
    Opie::DB::OSQLDriver* m_driver;
    QString m_logTable;
    QString m_peerTable;
    uint m_peerLastSyncLogId;
    bool m_slowSync;
    bool m_enabled;
    bool m_peersSynced;
};
}
#endif
