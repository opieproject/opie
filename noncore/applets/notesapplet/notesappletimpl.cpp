/**********************************************************************
** Copyright (C) 2002 L.J. Potter <llornkcor@handhelds.org>
**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
**
**********************************************************************/
#include "notes.h"
#include "notesappletimpl.h"


NotesAppletImpl::NotesAppletImpl()
    : notes(0), ref(0) {
}

NotesAppletImpl::~NotesAppletImpl() {
    delete notes;
}

QWidget *NotesAppletImpl::applet( QWidget *parent ) {
    if ( !notes )
  notes = new NotesApplet( parent );
    return notes;
}

int NotesAppletImpl::position() const {
    return 6;
}

QRESULT NotesAppletImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface ) {
    *iface = 0;
    if ( uuid == IID_QUnknown )
  *iface = this;
    else if ( uuid == IID_TaskbarApplet )
  *iface = this;

    if ( *iface )
  (*iface)->addRef();
    return QS_OK;
}

Q_EXPORT_INTERFACE() {
    Q_CREATE_INSTANCE( NotesAppletImpl )
}


