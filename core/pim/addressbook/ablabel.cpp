/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
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

#include "ablabel.h"

#include <qpe/stringutil.h>

#include <qregexp.h>
#include <qstylesheet.h>

AbLabel::AbLabel( QWidget *parent, const char *name )
  : QTextView( parent, name )
{
}

AbLabel::~AbLabel()
{
}

void AbLabel::init( const OContact &entry )
{
    ent = entry;
}

void AbLabel::sync()
{
    QString text = ent.toRichText();
    setText( text );
}

void AbLabel::keyPressEvent( QKeyEvent *e )
{
    if ( e->key() == Qt::Key_F33 ) {
	emit okPressed();
    }
}
