/**********************************************************************
** MGraph
**
** A reusable graph widget.
**
** Copyright (C) 2002, Michael Lauer
**                    mickey@tm.informatik.uni-frankfurt.de
**                    http://www.Vanille.de
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "mgraph.h"
#include "graphbackground.xpm"
#include <qpainter.h>
#include <qpixmap.h>

//---------------------------------------------------------------------------

MGraph::MGraph( QWidget *parent, const char *name, WFlags f )
    : QFrame( parent, name, f ), min( 0 ), max( 0), values( 0 )
{
    background = new QPixmap( (const char** ) graphbackground_xpm );
    values = new ValueList();
}

void MGraph::setFrameStyle( int style )
{
    QFrame::setFrameStyle( style );
    setFixedSize( background->width() + frameWidth()*2, background->height() + frameWidth()*2 );
}

void MGraph::addValue( int value, bool followMax )
{
    values->append( value );
    if ( followMax && (value > max) )
        setMax( value );
    if ( values->count() == background->width()-2 ) // compensate for graph display element border
        values->remove( values->begin() );
    repaint( false );
}

void MGraph::drawContents( QPainter* p )
{
    p->drawPixmap( frameWidth(), frameWidth(), (const QPixmap&) *background );
    p->setPen( QColor( 40, 235, 40 ) );

    int x = frameWidth() + 2; // compensate for graph display element border
    int y = 0;

    ValueList::ConstIterator it;
    for ( it = values->begin(); it != values->end(); ++it )
    {
        y = frameWidth() + background->height() -3 - ( ( *(it)*(background->height()-4 ) /max ) );
        p->drawPoint( x++, y );
    }
}

MGraph::~MGraph()
{
    delete background;
    delete values;
}

