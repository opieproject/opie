/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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

#include <qfile.h>
#include <qtl.h>
#include <math.h>
#include <limits.h>
#include <qdatastream.h>
#include "qimpencombining.h"

static unsigned int combiningSymbols[] = { '\\', '/', '^', '~', '\"', 'o' };
static unsigned int combiningChars[][7] = {
    //     \       /       ^       ~       "
    { 'A', 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5 },
    { 'O', 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x0000 },
    { 'U', 0x00D9, 0x00DA, 0x00DB, 0x0000, 0x00DC, 0x0000 },
    { 'E', 0x00C8, 0x00C9, 0x00CA, 0x0000, 0x00CB, 0x0000 },
    { 'I', 0x00CC, 0x00CD, 0x00CE, 0x0000, 0x00CF, 0x0000 },
    { 'a', 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5 },
    { 'e', 0x00E8, 0x00E9, 0x00EA, 0x0000, 0x00EB, 0x0000 },
    { 'i', 0x00EC, 0x00ED, 0x00EE, 0x0000, 0x00EF, 0x0000 },
    { 'n', 0x0000, 0x0000, 0x0000, 0x00F1, 0x0000, 0x0000 },
    { 'o', 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x0000 },
    { 'u', 0x00F9, 0x00FA, 0x00FB, 0x0000, 0x00FC, 0x0000 },
    { 'y', 0x0000, 0x00FD, 0x0000, 0x0000, 0x00FF, 0x0000 },
    { 0, 0, 0, 0, 0, 0, 0 }
};


QIMPenCombining::QIMPenCombining()
{
}

QIMPenCombining::QIMPenCombining( const QString &fn )
    : QIMPenCharSet( fn )
{
}

void QIMPenCombining::addCombined( QIMPenCharSet *cs )
{
    unsigned int count = cs->count();
    QIMPenCharIterator it( cs->characters() );
    for ( ; it.current() && count; ++it, --count ) {
	QIMPenChar *pc = it.current();
	if ( pc->testFlag( QIMPenChar::Deleted ) )
	    continue;
	int charIdx = findCombining( pc->character() );
	if ( charIdx < 0 )
	    continue;
	for ( int i = 0; i < 6; i++ ) {
	    if ( combiningChars[charIdx][i+1] ) {
		QIMPenCharIterator cit( chars );
		for ( ; cit.current(); ++cit ) {
		    QIMPenChar *accentPc = cit.current();
		    if ( accentPc->character() == combiningSymbols[i] ) {
			QIMPenChar *combined = combine( pc, accentPc );
			combined->setCharacter( combiningChars[charIdx][i+1] );
			cs->addChar( combined );
		    }
		}
	    }
	}
    }
}

int QIMPenCombining::findCombining( unsigned int ch ) const
{
    int i = 0;
    while ( combiningChars[i][0] ) {
	if ( combiningChars[i][0] == ch )
	    return i;
	i++;
    }

    return -1;
}

QIMPenChar *QIMPenCombining::combine( QIMPenChar *base, QIMPenChar *accent )
{
    QRect brect = base->boundingRect();
    QRect arect = accent->boundingRect();
    int offset;
    if ( accent->testFlag( QIMPenChar::CombineRight ) )
	offset = brect.left() - arect.left() + brect.width() + 2;
    else
	offset = brect.left() - arect.left() + (brect.width() - arect.width())/2;
    QIMPenChar *combined = 0;
    if ( base->character() == 'i' ) {
	// Hack to remove the dot from i's when combining.
	if ( base->penStrokes().count() > 1 ) {
	    combined = new QIMPenChar;
	    QIMPenStrokeIterator it( base->penStrokes() );
	    for ( unsigned int i = 0; i < base->penStrokes().count()-1; ++it, i++ ) {
		QIMPenStroke *st = new QIMPenStroke( *(it.current()) );
		combined->addStroke( st );
	    }
	    combined->setFlag( QIMPenChar::System );
	}
    }
    if ( !combined )
	combined = new QIMPenChar( *base );
    QIMPenStrokeIterator it( accent->penStrokes() );
    for ( ; it.current(); ++it ) {
	QIMPenStroke *st = new QIMPenStroke( *(it.current()) );
	st->setStartingPoint( st->startingPoint() + QPoint(offset, 0 ));
	combined->addStroke( st );
	delete st;
    }

    return combined;
}

QIMPenChar *QIMPenCombining::penChar( int type )
{
    QIMPenCharIterator it( chars );
    for ( ; it.current(); ++it ) {
	QIMPenChar *pc = it.current();
	if ( pc->character() == combiningSymbols[type] )
	    return pc;
    }

    return 0;
}

