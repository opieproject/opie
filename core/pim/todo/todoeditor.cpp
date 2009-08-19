/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
:     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include <opie2/odebug.h>

#include <qpe/qpeapplication.h>

#include "otaskeditor.h"
#include "todoeditor.h"

using namespace Opie;
using namespace Todo;

Editor::Editor()
{
    m_accepted = false;
    m_self = 0l;
}

Editor::~Editor()
{
    delete m_self;
    m_self = 0;
}

OPimTodo Editor::newTodo( int cur,
                           QWidget*)
{
    OTaskEditor *e = self();
    e->setCaption( QObject::tr("Enter Task") );
    e->init( cur );

    int ret = QPEApplication::execDialog( e );

    if ( QDialog::Accepted == ret )
        m_accepted = true;
    else
        m_accepted = false;

    OPimTodo ev = e->todo();
    ev.setUid(1);

    return ev;
}

OPimTodo Editor::edit( QWidget *,
                        const OPimTodo& todo )
{
    OTaskEditor *e = self();
    e->init( todo );
    e->setCaption( QObject::tr( "Edit Task" ) );

    int ret = QPEApplication::execDialog( e );

    OPimTodo ev = e->todo();
    if ( ret == QDialog::Accepted )
        m_accepted = true;
    else
        m_accepted = false;

    return ev;
}

bool Editor::accepted() const
{
    return m_accepted;
}

OTaskEditor* Editor::self()
{
    if ( !m_self )
        m_self = new OTaskEditor(0);

    return m_self;
}
