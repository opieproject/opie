/****************************************************************************
** $Id: qcomplextext.cpp,v 1.1 2002-07-14 21:21:35 leseb Exp $
**
** Implementation of some internal classes
**
** Created :
**
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qcomplextext_p.h"

#include "qrichtext_p.h"
#include "qfontmetrics.h"
#include "qrect.h"

#include <stdlib.h>

using namespace Qt3;

// -----------------------------------------------------

/* a small helper class used internally to resolve Bidi embedding levels.
   Each line of text caches the embedding level at the start of the line for faster
   relayouting
*/
QBidiContext::QBidiContext( uchar l, QChar::Direction e, QBidiContext *p, bool o )
    : level(l) , override(o), dir(e)
{
    if ( p )
	p->ref();
    parent = p;
    count = 0;
}

QBidiContext::~QBidiContext()
{
    if( parent && parent->deref() )
	delete parent;
}

static QChar *shapeBuffer = 0;
static int shapeBufSize = 0;

/*
   Arabic shaping obeys a number of rules according to the joining classes (see Unicode book, section on
   arabic).

   Each unicode char has a joining class (right, dual (left&right), center (joincausing) or transparent).
   transparent joining is not encoded in QChar::joining(), but applies to all combining marks and format marks.

   Right join-causing: dual + center
   Left join-causing: dual + right + center

   Rules are as follows (for a string already in visual order, as we have it here):

   R1 Transparent characters do not affect joining behaviour.
   R2 A right joining character, that has a right join-causing char on the right will get form XRight
   (R3 A left joining character, that has a left join-causing char on the left will get form XLeft)
   Note: the above rule is meaningless, as there are no pure left joining characters defined in Unicode
   R4 A dual joining character, that has a left join-causing char on the left and a right join-causing char on
	     the right will get form XMedial
   R5  A dual joining character, that has a right join causing char on the right, and no left join causing char on the left
	 will get form XRight
   R6 A dual joining character, that has a  left join causing char on the left, and no right join causing char on the right
	 will get form XLeft
   R7 Otherwise the character will get form XIsolated

   Additionally we have to do the minimal ligature support for lam-alef ligatures:

   L1 Transparent characters do not affect ligature behaviour.
   L2 Any sequence of Alef(XRight) + Lam(XMedial) will form the ligature Alef.Lam(XLeft)
   L3 Any sequence of Alef(XRight) + Lam(XLeft) will form the ligature Alef.Lam(XIsolated)

   The two functions defined in this class do shaping in visual and logical order. For logical order just replace right with
   previous and left with next in the above rules ;-)
*/

/*
  Two small helper functions for arabic shaping. They get the next shape causing character on either
  side of the char in question. Implements rule R1.

  leftChar() returns true if the char to the left is a left join-causing char
  rightChar() returns true if the char to the right is a right join-causing char
*/
static inline const QChar *prevChar( const QString &str, int pos )
{
    //qDebug("leftChar: pos=%d", pos);
    pos--;
    const QChar *ch = str.unicode() + pos;
    while( pos > -1 ) {
	if( !ch->isMark() )
	    return ch;
	pos--;
	ch--;
    }
    return &QChar::replacement;
}

static inline const QChar *nextChar( const QString &str, int pos)
{
    pos++;
    int len = str.length();
    const QChar *ch = str.unicode() + pos;
    while( pos < len ) {
	//qDebug("rightChar: %d isLetter=%d, joining=%d", pos, ch.isLetter(), ch.joining());
	if( !ch->isMark() )
	    return ch;
	// assume it's a transparent char, this might not be 100% correct
	pos++;
	ch++;
    }
    return &QChar::replacement;
}

static inline bool prevVisualCharJoins( const QString &str, int pos)
{
    return (     prevChar( str, pos )->joining() != QChar::OtherJoining );
}

static inline bool nextVisualCharJoins( const QString &str, int pos)
{
    QChar::Joining join = nextChar( str, pos )->joining();
    return ( join == QChar::Dual || join == QChar::Center );
}
