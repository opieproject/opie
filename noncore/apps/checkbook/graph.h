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

#ifndef GRAPH_H
#define GRAPH_H

#include <qpixmap.h>
#include <qwidget.h>

class GraphInfo;
class QPainter;

class Graph : public QWidget
{
	Q_OBJECT

	public:
		Graph( QWidget * = 0x0, GraphInfo * = 0x0, const QString & = 0x0, int = 0 );

		void setGraphInfo( GraphInfo * );

		void drawGraph( bool = FALSE );

	protected:
		void paintEvent( QPaintEvent * );
		void resizeEvent( QResizeEvent * );

	private:
		GraphInfo *data;

		QPixmap graph;

		void initGraph();
		void drawBarChart( int, int, float );
		void drawPieChart( int, int, float );
};

#endif
