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
#ifndef NOTESAPPLETIMPL_H
#define NOTESAPPLETIMPL_H

#include <qpe/taskbarappletinterface.h>

class NotesApplet;

class NotesAppletImpl : public TaskbarAppletInterface
{
public:
    NotesAppletImpl();
    virtual ~NotesAppletImpl();

    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT

    virtual QWidget *applet( QWidget *parent );
    virtual int position() const;

private:
    NotesApplet *notes;
    ulong ref;
};

#endif
