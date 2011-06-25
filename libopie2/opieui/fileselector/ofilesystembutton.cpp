/*
                             This file is part of the Opie Project
               =.            Copyright (C) 2005 Holger Freyther <freyther@handhelds.org>
             .=l.            Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can 
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.        
    .i_,=:_.      -<s.       This library is distributed in the hope that  
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

#include <opie2/ofilesystembutton.h>

/* Std */
#include <stdlib.h>

/* Qt */
#include <qpopupmenu.h>
#include <qlist.h>

/* Opie */
#include <opie2/oresource.h>
#include <opie2/ostorageinfo.h>

using namespace Opie::Ui;
using namespace Opie::Core;

OFileSystemButton::OFileSystemButton( QWidget *parent, bool showHome )
    : QToolButton( parent )
{
    m_showHome = showHome;

    setPixmap( OResource::loadPixmap( "pcmcia", OResource::SmallIcon ) );

    m_popup = new QPopupMenu(this);
    connect(m_popup, SIGNAL( activated(int) ),
            this, SLOT(slotFSActivated(int) ) );

    m_storage = new OStorageInfo();
    connect(m_storage, SIGNAL(disksChanged() ),
            this, SLOT( slotDisksChanged() ) );
    slotDisksChanged();

    connect(this, SIGNAL(pressed()), SLOT(slotPopUp()) );
}

OFileSystemButton::~OFileSystemButton()
{
    delete m_storage;
}

void OFileSystemButton::slotDisksChanged()
{
    if( m_showHome ) {
        QString hdir = getenv( "HOME" );
        if (!hdir.isEmpty()) {
            QString htext = tr( "Home directory" );
            m_dev.insert(htext,hdir);
            m_popup->insertItem(htext);
        }
    }

    const QList<FileSystem> &fs = m_storage->fileSystems();
    QListIterator<FileSystem> it(fs);
    for ( ; it.current(); ++it )
    {
        const QString disk = (*it)->name();
        const QString path = (*it)->path();
        m_dev.insert( disk, path );
        m_popup->insertItem( disk );
    }
}

void OFileSystemButton::slotPopUp() {
    m_popup->exec( mapToGlobal( QPoint(0, height()) ) );
    setDown( false );
}

void OFileSystemButton::slotFSActivated( int id )
{
    emit changeDir( m_dev[m_popup->text(id)] );
}
