/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 Dan Williams <williamsdr@acm.org>
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

#include <qpainter.h>

Graph::Graph( QWidget *parent, GraphInfo *d, const QString &name, int flags )
	: QWidget( parent, name, flags )
{
	setBackgroundMode( QWidget::PaletteBase );

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
			drawBarChart();
		}
		break;
		case GraphInfo::LineChart :
		{
			drawLineChart();
		}
		break;
		case GraphInfo::PieChart :
		{
			drawPieChart();
		}
	};
}

void Graph::drawBarChart()
{
	//Find max value in GraphInfo->dataPoints() - make function in GraphInfo!!!
}

void Graph::drawLineChart()
{
}

void Graph::drawPieChart()
{
}

