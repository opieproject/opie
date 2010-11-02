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

#include "opimresolver.h"

/* OPIE */
#include <opie2/ocontactaccess.h>
#include <opie2/otodoaccess.h>
#include <opie2/omemoaccess.h>
#include <qpe/qcopenvelope_qws.h>

/* QT */
#include <qcopchannel_qws.h>

namespace Opie {

OPimResolver* OPimResolver::m_self = 0;

OPimResolver::OPimResolver() {
    /* the built in channels */
    m_builtIns << "Todolist" << "Addressbook" << "Datebook" << "Memo";
}

OPimResolver* OPimResolver::self() {
    if (!m_self)
        m_self = new OPimResolver();

    return m_self;
}

/*
 * FIXME use a cache here too
 */
OPimRecord* OPimResolver::record( const QString& service, int uid ) {
    OPimRecord* rec = 0;
    OPimBase* base = backend( service );

    if ( base )
        rec = base->record( uid );

    delete base;

    return rec;
}

OPimRecord* OPimResolver::record( const QString& service ) {
    return record( serviceId( service ) );
}

OPimRecord* OPimResolver::record( int rtti ) {
    OPimRecord* rec = 0;
    switch( rtti ) {
        case 1: /* todolist */
            rec = new OPimTodo();
            break;
        case 2: /* contact  */
            rec = new OPimContact();
            break;
        default:
            break;
    }
    return rec;
}

bool OPimResolver::isBuiltIn( const QString& str) const{
    return m_builtIns.contains( str );
}

QCString OPimResolver::qcopChannel( enum BuiltIn& built)const {
    QCString str("QPE/");
    switch( built ) {
    case TodoList:
        str += "Todolist";
        break;
    case DateBook:
        str += "Datebook";
        break;
    case AddressBook:
        str += "Addressbook";
        break;
    case Notes:
        str += "Notes";
        break;
    default:
        break;
    }

    return str;
}

QCString OPimResolver::qcopChannel( const QString& service )const {
    QCString str("QPE/");
    str += service.latin1();
    return str;
}

/*
 * Implement services!!
 * FIXME
 */
QCString OPimResolver::applicationChannel( enum BuiltIn& built)const {
    QCString str("QPE/Application/");
    switch( built ) {
    case TodoList:
        str += "todolist";
        break;
    case DateBook:
        str += "datebook";
        break;
    case AddressBook:
        str += "addressbook";
        break;
    case Notes:
        str += "notes";
        break;
    }

    return str;
}

QCString OPimResolver::applicationChannel( const QString& service )const {
    QCString str("QPE/Application/");

    if ( isBuiltIn( service ) ) {
        if ( service == "Todolist" )
            str += "todolist";
        else if ( service == "Datebook" )
            str += "datebook";
        else if ( service == "Addressbook" )
            str += "addressbook";
        else if ( service == "Notes" )
            str += "notes";
    }
    // FIXME Add Support for 3rd party

    return str;
}

QStringList OPimResolver::services()const {
    return m_builtIns;
}

QString OPimResolver::serviceName( int rtti ) const{
    QString str;
    switch ( rtti ) {
    case TodoList:
        str = "Todolist";
        break;
    case DateBook:
        str = "Datebook";
        break;
    case AddressBook:
        str = "Addressbook";
        break;
    case Notes:
        str = "Notes";
        break;
    default:
        break;
    }
    return str;
    // FIXME Add support for 3rd party
}

int OPimResolver::serviceId( const QString& service ) {
    int rtti = 0;
    if ( service == "Todolist" )
        rtti = TodoList;
    else if ( service == "Datebook" )
        rtti = DateBook;
    else if ( service == "Addressbook" )
        rtti = AddressBook;
    else if ( service == "Notes" )
        rtti = Notes;

    return rtti;
}

/**
 * Check if the 'service' is registered and if so we'll
 */
bool OPimResolver::add( const QString& service,  const OPimRecord& rec) {
    if ( QCopChannel::isRegistered( applicationChannel( service ) ) ) {
        QByteArray data;
        QDataStream arg(data, IO_WriteOnly );
        if ( rec.saveToStream( arg ) ) {
            QCopEnvelope env( applicationChannel( service ), "add(int,QByteArray)" );
            env << rec.rtti();
            env << data;
        }else
            return false;
    }else{
        OPimBase* base = backend( service );
        if (!base || base->load())
	    return false;

        base->add( rec );
        base->save();
        delete base;
    }

    return true;
}
OPimBase* OPimResolver::backend( const QString& service ) {
    return backend( serviceId( service ) );
}
OPimBase* OPimResolver::backend( int rtti ) {
    OPimBase* base = 0l;
    switch( rtti ) {
    case TodoList:
        base = new OPimTodoAccess();
        break;
    case DateBook:
        break;
    case AddressBook:
        base = new OPimContactAccess("Resolver");
        break;
    case Notes:
        base = new OPimMemoAccess("Resolver");
        break;
    default:
        break;
    }
    // FIXME Add support for 3rd party

    return base;
}

}
