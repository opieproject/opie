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

#ifndef SYNC_ACCESS_MANAGER_H
#define SYNC_ACCESS_MANAGER_H

#include <opie2/ocontactaccess.h>
#include <opie2/odatebookaccess.h>
#include <opie2/otodoaccess.h>
#include <opie2/omemoaccess.h>
#include <opie2/opimglobal.h>
#include <opie2/opimchangelog.h>
#include <opie2/opimaccessfactory.h>

#include <qmap.h>

class SyncAccessManager {
public:
    SyncAccessManager();
    virtual ~SyncAccessManager();
    
    void setPeerInfo( const QString &peerId, const QString &peerName );
    bool startSync( const QString &app, bool slowSyncRead, bool slowSyncWrite );
    void syncDone( const QString &app );
    bool isSlowSyncWrite( const QString &app );
    bool hasChangeLog( const QString &app );

    OPimContactAccess *contactAccess();
    ODateBookAccess *dateBookAccess();
    OPimTodoAccess *todoAccess();
    OPimMemoAccess *memoAccess();

private:
    OPimBase *appAccess( const QString &app );
    
    QString m_peerId;
    QString m_peerName;
    OPimContactAccess *m_contactAccess;
    ODateBookAccess *m_dateBookAccess;
    OPimTodoAccess *m_todoAccess;
    OPimMemoAccess *m_memoAccess;
    QMap<QString,bool> m_slowSyncWrite;
};

#endif
