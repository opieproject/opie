/* This file is part of the KDE project
   Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2002 Joseph Wenninger <jowenn@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include "ktexteditor.h"

using namespace KTextEditor;

class View::ViewPrivate
{
public:
  ViewPrivate()
  {
  }
  ~ViewPrivate()
  {
  }

  Document *m_doc;
  bool m_bContextPopup;
};

View::View( Document *doc, QWidget *parent, const char *name )
: QWidget( parent, name )
{
  d = new ViewPrivate;
  d->m_doc = doc;
  d->m_bContextPopup = true;
}

View::~View()
{
  delete d;
}

Document *View::document() const
{
  return d->m_doc;
}

void View::insertText( const QString &text, bool mark )
{
  int line, col;
  getCursorPosition( &line, &col );
  document()->insertAt( text, line, col, mark );
} 

void View::setInternalContextMenuEnabled( bool b )
{
  d->m_bContextPopup = b;
}

bool View::internalContextMenuEnabled() const
{
  return d->m_bContextPopup;
}

class Document::DocumentPrivate
{
public:
  DocumentPrivate()
  {
  }
  ~DocumentPrivate()
  {
  }

};

Document::Document( QObject *parent, const char *name )
 : QObject(parent,name)
{
  d = new DocumentPrivate;
}

Document::~Document()
{
  //one never knows...
  QListIterator<View> it( m_views );

  for (; it.current(); ++it )
    disconnect( it.current(), SIGNAL( destroyed() ),
		this, SLOT( slotViewDestroyed() ) );

  delete d;
}

QList<View> Document::views() const
{
  return m_views;
}

void Document::addView( View *view )
{
  if ( !view )
    return;

  if ( m_views.findRef( view ) != -1 )
    return;

  m_views.append( view );
  connect( view, SIGNAL( destroyed() ),
	   this, SLOT( slotViewDestroyed() ) );
}

void Document::removeView( View *view )
{
  if ( !view )
    return;

  disconnect( view, SIGNAL( destroyed() ),
	      this, SLOT( slotViewDestroyed() ) );

  m_views.removeRef( view );
}

void Document::slotViewDestroyed()
{
  const View *view = static_cast<const View *>( sender() );
  m_views.removeRef( view );
}


