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

#include <qpainter.h>
#include "qimpenwordpick.h"

QIMPenWordPick::QIMPenWordPick( QWidget *parent, const char *name, WFlags f )
 : QFrame( parent, name, f )
{
    clickWord = -1;
    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );
}

void QIMPenWordPick::clear()
{
    words.clear();
    repaint();
}

QSize QIMPenWordPick::sizeHint() const
{
    return QSize( -1, font().pixelSize()+2 );
}

void QIMPenWordPick::setWords( const QIMPenMatch::MatchWordList &w )
{
    words.clear();
    QListIterator<QIMPenMatch::MatchWord> it( w );
    for ( ; it.current(); ++it ) {
	words.append( it.current()->word );
    }
    repaint();
}

int QIMPenWordPick::onWord( QPoint p )
{
    int x = 2;
    int idx = 0;
    for ( QStringList::Iterator it = words.begin(); it != words.end(); ++it ) {
	QString word = *it;
	int w = fontMetrics().width( word );
	if ( x + w > width() )
	    break;
	if ( p.x() > x-2 && p.x() < x + w + 2 )
	    return idx;
	x += w + 5;
	if ( !idx )
	    x += 3;
	idx++;
    }

    return -1;
}

void QIMPenWordPick::paintEvent( QPaintEvent * )
{
    QPainter p(this);
    int x = 2;
    int h = p.fontMetrics().ascent() + 1;
    int idx = 0;
    for ( QStringList::Iterator it = words.begin(); it != words.end(); ++it ) {
	QString word = *it;
	int w = p.fontMetrics().width( word );
	if ( x + w > width() )
	    break;
	if ( idx == clickWord ) {
	    p.fillRect( x, 0, w, height(), black );
	    p.setPen( white );
	} else {
	    p.setPen( colorGroup().text() );
	}
	p.drawText( x, h, word );
	x += w + 5;
	if ( !idx )
	    x += 3;
	idx++;
    }
}

void QIMPenWordPick::mousePressEvent( QMouseEvent *e )
{
    clickWord = onWord( e->pos() );
    repaint();
}

void QIMPenWordPick::mouseReleaseEvent( QMouseEvent *e )
{
    int wordIdx = onWord( e->pos() );
    if ( wordIdx >= 0 && wordIdx == clickWord ) {
	//qDebug( "Clicked %s", words[wordIdx].latin1() );
	emit wordClicked( words[wordIdx] );
    }
    clickWord = -1;
    repaint();
}

