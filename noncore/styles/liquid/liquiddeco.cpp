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

#include <qapplication.h>
#include <qstyle.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qwhatsthis.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpedecoration_qws.h>
#include <qdialog.h>
#include <qdrawutil.h>
#include <qgfx_qws.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/global.h>
#include <qfile.h>
#include <qsignal.h>

#include "liquiddeco.h"

#include <stdlib.h>

extern QRect qt_maxWindowRect;

class HackWidget : public QWidget
{
public:
	bool needsOk()
	{
		return ( getWState() & WState_Reserved1 );
	}
};


LiquidDecoration::LiquidDecoration()
		: QPEDecoration()
{}

LiquidDecoration::~LiquidDecoration()
{}

int LiquidDecoration::getTitleHeight( const QWidget * )
{
	return 15;
}


void LiquidDecoration::paint( QPainter *painter, const QWidget *widget )
{
	int titleWidth = getTitleWidth( widget );
	int titleHeight = getTitleHeight( widget );

	QRect rect( widget->rect() );

	// Border rect
	QRect br( rect.left() - BORDER_WIDTH,
	          rect.top() - BORDER_WIDTH - titleHeight,
	          rect.width() + 2 * BORDER_WIDTH,
	          rect.height() + BORDER_WIDTH + BOTTOM_BORDER_WIDTH + titleHeight );

	// title bar rect
	QRect tr;

	tr = QRect( rect.left(), rect.top() - titleHeight, rect.width(), titleHeight );

	QRegion oldClip = painter->clipRegion();
	painter->setClipRegion( oldClip - QRegion( tr ) );	// reduce flicker

	bool isActive = ( widget == qApp->activeWindow() );

	QColorGroup cg = QApplication::palette().active();
	if ( isActive )
		cg. setBrush ( QColorGroup::Button, cg. brush ( QColorGroup::Highlight ) );

	qDrawWinPanel( painter, br.x(), br.y(), br.width(),
	               br.height() - 4, cg, FALSE,
	               &cg.brush( QColorGroup::Background ) );

	painter->setClipRegion( oldClip );

	if ( titleWidth > 0 ) {
		QBrush titleBrush;
		QPen titlePen;
		QPen titleLines;
		int titleLeft = titleHeight + 4;

		titleLeft = rect.left() + 5;
		painter->setPen( cg.midlight() );
		painter->drawLine( rect.left() - BORDER_WIDTH + 2,
		                   rect.bottom() + 1, rect.right() + BORDER_WIDTH - 2,
		                   rect.bottom() + 1 );

		QRect t ( rect.left() - 2, rect.top() - titleHeight - 2, rect.width() + 3, titleHeight + 2 );



		QApplication::style().drawBevelButton( painter, t.x(), t.y(), t.width(), t.height(), cg, isActive );

		t.setLeft( t.left() + 4 );
		t.setRight( t.right() - 2 );

		QFont f( QApplication::font() );
		f.setWeight( QFont::Bold );

		painter-> setFont( f );

		QColor textcol = cg.color( isActive ? QColorGroup::HighlightedText : QColorGroup::Text );
		QColor shadecol = ( qGray ( textcol. rgb ( ) ) > 128 ) ? textcol. dark ( 130 ) : textcol.light( 200 );

		if ( textcol == shadecol ) {
			if ( qGray ( shadecol. rgb ( ) ) < 128 )
				shadecol = QColor ( 225, 225, 225 );
			else
				shadecol = QColor ( 30, 30, 30 );
		}

		painter-> setPen( shadecol );
		painter-> drawText( t.x() + 1, t.y() + 1, t.width(), t.height(), Qt::AlignLeft | Qt::AlignVCenter | Qt::SingleLine, widget->caption() );
		painter-> setPen( textcol );
		painter-> drawText( t.x(), t.y(), t.width(), t.height(), Qt::AlignLeft | Qt::AlignVCenter | Qt::SingleLine, widget->caption() );
	}

#ifndef MINIMIZE_HELP_HACK
	paintButton( painter, widget, ( QWSDecoration::Region ) Help, 0 );
#endif
}

void LiquidDecoration::paintButton( QPainter *painter, const QWidget *w,
                                    QWSDecoration::Region type, int state )
{
	const QColorGroup & cg = w->palette().active();

	QRect brect( region( w, w->rect(), type ).boundingRect() );

	const QImage *img = 0;

	switch ( ( int ) type ) {
		case Close:
			img = &imageClose;
			break;
		case Minimize:
			if ( ( ( HackWidget * ) w ) ->needsOk() ||
			        ( w->inherits( "QDialog" ) && !w->inherits( "QMessageBox" ) ) )
				img = &imageOk;
			else if ( helpExists )
				img = &imageHelp;
			break;
		case Help:
			img = &imageHelp;
			break;
		default:
			return ;
	}

	if ( img ) {
		if ( ( state & QWSButton::MouseOver ) && ( state & QWSButton::Clicked ) ) 
			painter->drawImage( brect.x() + 1, brect.y() + 3, *img );
		else 
			painter->drawImage( brect.x(), brect.y() + 2, *img );
	}
}



