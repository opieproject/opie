/*
 *
 * $Id: qrestrictedline.cpp,v 1.3 2002-09-15 17:23:21 kergoth Exp $
 *
 * Implementation of QRestrictedLine
 *
 * Copyright (C) 1997 Michael Wiedmann, <mw@miwie.in-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <qkeycode.h>

#include "qrestrictedline.h"

QRestrictedLine::QRestrictedLine( QWidget *parent, 
				  const char *name,
				  const QString& valid )
  : QLineEdit( parent, name )
{
    qsValidChars = valid;
}

QRestrictedLine::~QRestrictedLine()
{
  ;
}


void QRestrictedLine::keyPressEvent( QKeyEvent *e )
{
  // let QLineEdit process "special" keys and return/enter
  // so that we still can use the default key binding
  if (e->key() == Key_Enter || e->key() == Key_Return || e->key() == Key_Delete || e->ascii() < 32)
    {
      QLineEdit::keyPressEvent(e);
      return;
    }

  // do we have a list of valid chars &&
  // is the pressed key in the list of valid chars?
  if (!qsValidChars.isEmpty() && !qsValidChars.contains(e->ascii()))
    {
      // invalid char, emit signal and return
      emit (invalidChar(e->key()));
      return;
    }
  else
	// valid char: let QLineEdit process this key as usual
	QLineEdit::keyPressEvent(e);

  return;
}


void QRestrictedLine::setValidChars( const QString& valid)
{
  qsValidChars = valid;
}

QString QRestrictedLine::validChars() const
{
  return qsValidChars;
}

