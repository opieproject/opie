/**********************************************************************
** Copyright (C) 2002 by Stefan Eilers (se, eilers.stefan@epost.de)
** Copyright (C) 2002 Holger 'zecke' Freyther <freyther@kde.org>
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/



#include <qpe/stringutil.h>

#include <qregexp.h>
#include <qstylesheet.h>


#include "todolabel.h"

TodoLabel::TodoLabel( QWidget *parent, const char *name )
  : QTextView( parent, name )
{
}

TodoLabel::~TodoLabel()
{
}

void TodoLabel::init(  const ToDoEvent &item )
{
    m_item = item;
}

void TodoLabel::sync()
{
  QString text = m_item.richText();
  setText( text );
}

void TodoLabel::keyPressEvent( QKeyEvent *e )
{
    if ( e->key() == Qt::Key_F33 ) {
	emit okPressed();
    }
}
