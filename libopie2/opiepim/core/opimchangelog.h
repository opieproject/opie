/*
                             This file is part of the Opie Project
                             Copyright (C) 2009 Paul Eggleton <bluelightning@bluelightning.org>
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

#ifndef OPIMCHANGELOG_H
#define OPIMCHANGELOG_H

#include <qstring.h>
#include <qdatetime.h>
#include <qlist.h>

namespace Opie
{

class OPimSyncPeer;
    
/**
 * Abstract class for dealing with PIM database change logging
 *
 * A change log is an optional part of a backend. If implemented it stores
 * entries tracking changes to the data (adds, updates, deletes).
 * It also holds a separate list of peers it has synced with and when
 * they last synced (so that (a) we can filter for changes since each
 * peer last synced, and (b) we can know when it's safe to delete entries
 * that are no longer needed by any peer.) Also provided is a way to find
 * out when a peer last synced and remove a peer so that log entries only
 * useful to an "old" peer can be removed.
 *
 */
class OPimChangeLog
{
public:
    virtual ~OPimChangeLog() {}
    /**
     * Initialise the changelog
     */
    virtual void init() = 0;

    /**
     * Add an ADD entry to the log
     * @param uid the unique identifier of the record that was added
     */
    virtual void addAddEntry( int uid ) = 0;

    /**
     * Add an UPDATE entry to the log
     * @param uid the unique identifier of the record that was updated
     */
    virtual void addUpdateEntry( int uid ) = 0;

    /**
     * Add a DELETE entry to the log
     * @param uid the unique identifier of the record that was deleted
     */
    virtual void addDeleteEntry( int uid ) = 0;

    /**
     * Delete all information in the log
     */
    virtual void purge() = 0;

    /**
     * List peers
     * @returns a list of peer information objects
     */
    virtual QList<OPimSyncPeer> peerList() = 0;

    /**
     * Remove the specified peer
     * Removes the specified peer's last sync information and then triggers
     * a purge of the log for entries that are older than when the oldest peer
     * last synced (ie, entries that are no longer required)
     * @param peerId the unique identifier of the peer to remove
     */
    virtual void removePeer( const QString &peerId ) = 0;

    /**
     * Set the peer to filter for when querying the backend.
     * When set, only items logged as being changed after the specified peer
     * last successfully synced will be returned by the backend. This peer is also
     * used when syncDone() is called.
     * @param peer peer info structure for the peer we are synchronising with.
     *   (lastSyncDate is not expected to be populated.)
     * @param slowsync true to force a slow-sync, false otherwise
     * @returns true if we are able to filter for the specified peer, false if slow sync will be required
     */
    virtual bool startSync( const OPimSyncPeer &peer, bool slowSync ) = 0;

    /**
     * Indicate that synchronisation with the peer specified in startSync() was successful.
     * Records the last sync date and log ID for the peer and then triggers
     * a purge of the log for entries that are older than when the oldest peer
     * last synced (ie, entries that are no longer required)
     */
    virtual void syncDone() = 0;
};

class OPimSyncPeer
{
public:
    /**
     * Constructs a new peer info structure
     * @param peerId a unique identifier string for the peer that remains the same between synchronisation sessions
     * @param peerName a human-readable name for the peer (eg. the hostname). May change between sessions.
     * @param lastSyncDate (internal) date of the last successful sync with this peer
     */
    OPimSyncPeer( const QString &peerId, const QString &peerName, const QDateTime &lastSyncDate = QDateTime() )
        : m_peerId( peerId ), m_peerName( peerName ), m_lastSyncDate( lastSyncDate ) {}
    OPimSyncPeer() {}
    QString peerId() { return m_peerId; }
    QString peerName() { return m_peerName; }
    QDateTime lastSyncDate() { return m_lastSyncDate; }

private:
    QString m_peerId;
    QString m_peerName;
    QDateTime m_lastSyncDate;
};
}
#endif
