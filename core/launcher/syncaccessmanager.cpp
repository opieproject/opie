/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
            .=l.             
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this file; see the file COPYING.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "syncaccessmanager.h"

#include <opie2/odebug.h>

SyncAccessManager::SyncAccessManager()
    : m_contactAccess(0), m_dateBookAccess(0), m_todoAccess(0), m_memoAccess(0)
{
}

SyncAccessManager::~SyncAccessManager()
{
    reset();
}

void SyncAccessManager::reset()
{
    if( m_contactAccess ) {
        delete m_contactAccess;
        m_contactAccess = NULL;
    }
    if( m_dateBookAccess ) {
        delete m_dateBookAccess;
        m_dateBookAccess = NULL;
    }
    if( m_todoAccess ) {
        delete m_todoAccess;
        m_todoAccess = NULL;
    }
    if( m_memoAccess ) {
        delete m_memoAccess;
        m_memoAccess = NULL;
    }

    m_slowSyncWrite.clear();

    m_peerId = QString::null;
    m_peerName = QString::null;
}

void SyncAccessManager::setPeerInfo( const QString &peerId, const QString &peerName )
{
    m_peerId = peerId;
    m_peerName = peerName;
}

bool SyncAccessManager::startSync( const QString &app, bool slowSyncRead, bool slowSyncWrite )
{
    // Note: function returns true if slow-sync is required (or error)
    OPimBase *access = appAccess( app );
    if( access ) {
        if ( m_peerId.isEmpty() ) {
            owarn << "startSync: no peer ID has been set, can't start sync" << oendl;
            return true;
        }
        else {
            m_slowSyncWrite[ app ] = slowSyncWrite;
            OPimChangeLog *changelog = access->changeLog();
            if( changelog ) {
                changelog->init();
                OPimSyncPeer peer( m_peerId, m_peerName );
                return changelog->startSync( peer, slowSyncRead );
            }
            else
                return true;
        }
    }
    else {
        owarn << "startSync: unrecognised application name \"" << app << "\"" << oendl;
        return true;
    }
}

void SyncAccessManager::syncDone( const QString &app )
{
    OPimBase *access = appAccess( app );
    if( access ) {
        OPimChangeLog *changelog = access->changeLog();
        if( changelog )
            changelog->syncDone();
        // Dispose of access object since we are done with it
        freeAppAccess( app );
    }
    else
        owarn << "syncDone: unrecognised application name \"" << app << "\"" << oendl;
}

bool SyncAccessManager::isSlowSyncWrite( const QString &app )
{
    if( m_slowSyncWrite.contains( app ) )
        return m_slowSyncWrite[ app ];
    else
        return true;
}

bool SyncAccessManager::hasChangeLog( const QString &app )
{
    OPimBase *access = appAccess( app );
    if( access ) {
        OPimChangeLog *changelog = access->changeLog();
        if( changelog )
            return true;
    }
    else
        owarn << "hasChangeLog: unrecognised application name \"" << app << "\"" << oendl;

    return false;
}

OPimContactAccess *SyncAccessManager::contactAccess()
{
    if( !m_contactAccess )
        m_contactAccess = new OPimContactAccess();
    return m_contactAccess;
}

ODateBookAccess *SyncAccessManager::dateBookAccess()
{
    if( !m_dateBookAccess )
        m_dateBookAccess = new ODateBookAccess();
    return m_dateBookAccess;
}

OPimTodoAccess *SyncAccessManager::todoAccess()
{
    if( !m_todoAccess )
        m_todoAccess = new OPimTodoAccess();
    return m_todoAccess;
}

OPimMemoAccess *SyncAccessManager::memoAccess()
{
    if( !m_memoAccess )
        m_memoAccess = new OPimMemoAccess();
    return m_memoAccess;
}

OPimBase *SyncAccessManager::appAccess( const QString &app )
{
    QString appid = app.lower();
    if( appid == "addressbook" ) {
        return contactAccess();
    }
    else if( appid == "datebook" ) {
        return dateBookAccess();
    }
    else if( appid == "todolist" ) {
        return todoAccess();
    }
    else if( appid == "notes" ) {
        return memoAccess();
    }
    else
        return NULL;
}

void SyncAccessManager::freeAppAccess( const QString &app )
{
    QString appid = app.lower();
    if( appid == "addressbook" ) {
        if( m_contactAccess ) {
            delete m_contactAccess;
            m_contactAccess = NULL;
        }
    }
    else if( appid == "datebook" ) {
        if( m_dateBookAccess ) {
            delete m_dateBookAccess;
            m_dateBookAccess = NULL;
        }
    }
    else if( appid == "todolist" ) {
        if( m_todoAccess ) {
            delete m_todoAccess;
            m_todoAccess = NULL;
        }
    }
    else if( appid == "notes" ) {
        if( m_memoAccess ) {
            delete m_memoAccess;
            m_memoAccess = NULL;
        }
    }
}
