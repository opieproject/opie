/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 Dan Williams <drw@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "graph.h"
#include "graphinfo.h"

#include <qcolor.h>
#include <qfontmetrics.h>
#include <qpainter.h>

#include <math.h>

Graph::Graph( QWidget *parent, GraphInfo *d, const QString &name, int flags )
	: QWidget( parent, name, flags )
{
	data = d;

	graph.setOptimization( QPixmap::BestOptim );
}

void Graph::setGraphInfo( GraphInfo *d )
{
	data = d;
}

void Graph::drawGraph( bool regen )
{
	if ( regen )
	{
		initGraph();
	}
	QPainter p( this );
	p.drawPixmap( 0, 0, graph );
}

void Graph::paintEvent( QPaintEvent * )
{
	drawGraph( FALSE );
}

void Graph::resizeEvent( QResizeEvent * )
{
	drawGraph( TRUE );
}

void Graph::initGraph()
{
	graph.resize( width(), height() );
	graph.fill( QColor( 255, 255, 255 ) );

	if ( !data )
	{
		return;
	}

	// Any common stuff here (titles, ???)

	switch ( data->graphType() )
	{
		case GraphInfo::BarChart :
		{
			drawBarChart( width(), height(), data->maxValue() );
		}
		break;
		case GraphInfo::LineChart :
		{
			//drawLineChart( p, s, min, max );
		}
		break;
		case GraphInfo::PieChart :
		{
			drawPieChart( width(), height(), data->totalValue() );
		}
	};
}

void Graph::drawBarChart( int width, int height, float max )
{
	QPainter p( &graph );

	// Try to set the font size smaller for text
	QFont f = font();
	f.setPointSize( 8 );
	p.setFont( f );

	int x = 0;
	int i = 0;
	int n = data->numberDataPoints();
	QFontMetrics fm=fontMetrics();
	int fh = fm.height();
	int fw;

	QColor c( 0, 0, 255);
	p.setBrush( c );

	for (DataPointInfo *dp = data->firstDataPoint(); dp; dp = data->nextDataPoint() )
	{
		int bw = ( width - width / 4 - x ) / ( n - i );
		int bh = int( ( height - height / 4 - 1 ) * dp->value() / max );
		p.drawRect( width / 8 + x, height - height / 8 - 1 - bh, bw, bh );
		fw = fm.width( dp->label() );
		p.drawText( width / 8 + x - fw / 2 + bw / 2, height - height / 8, fw,
					fh + height / 8, AlignTop | AlignHCenter, dp->label() );
//					WordBreak | AlignTop | AlignHCenter, dp->label() );
		i++;
		x += bw;
	}
}

void Graph::drawLineChart( int width, int height, float max )
{
}

void Graph::drawPieChart( int width, int height, float sum )
{
	QPainter p( &graph );

	// Try to set the font size smaller for text
	QFont f = font();
	f.setPointSize( 8 );
	p.setFont( f );

	int n = data->numberDataPoints();

	int apos = -90 * 16;

	int xd = width - width / 5;
	int yd = height - height / 5;

	int i = 0;

	QColor c;

	for (DataPointInfo *dp = data->firstDataPoint(); dp; dp = data->nextDataPoint() )
	{
		c.setHsv( ( i *255) / n, 255, 255 );
		p.setBrush( c );

		int a = int( ( dp->value() * 360.0 ) / sum * 16.0 + 0.5 );
		p.drawPie( width/10, height/10, xd, yd, -apos, -a );
		apos += a;
		i++;
	}

	double apos2 = -90 * 3.14159 / 180;
	for (DataPointInfo *dp = data->firstDataPoint(); dp; dp = data->nextDataPoint() )
	{
		double a = dp->value() *360 / sum * 3.14159 / 180;
		int x = int( cos( apos2 + a/2 ) * width * 5/16 + width/2 + 0.5 );
		int y = int( sin( apos2 + a/2 ) * height * 5/16 + height/2 + 0.5 );
		p.drawText( x - width/8, y - height/8, width/4, height/4, WordBreak | AlignCenter,
					dp->label() );
		apos2 += a;
	}
}

