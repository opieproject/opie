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

#ifndef GRAPHINFO_H
#define GRAPHINFO_H

#include <qlist.h>
#include <qstringlist.h>

class DataPointInfo
{
	public:
		DataPointInfo()
			: l( 0x0 ), v( 0.0 ) {}
		DataPointInfo( const QString &label, float value )
			: l( label ), v( value ) {}

		const QString &label() { return l; }
		float          value() { return v; }

	private:
		QString l;
		float   v;
};

typedef QList<DataPointInfo> DataPointList;

class GraphInfo
{
	public:
		enum GraphType { BarChart, LineChart, PieChart };

		GraphInfo( GraphType = BarChart, DataPointList * = 0x0,
				   const QString & = 0x0, const QString & = 0x0, const QString & = 0x0 );

		GraphInfo::GraphType graphType();
		void                 setGraphType( GraphType );

		DataPointList *dataPoints();
		void           setDataPoints( DataPointList * );

		float maxValue();
		float minValue();

		void setGraphTitle( const QString & );
		void setXAxisTitle( const QString & );
		void setYAxisTitle( const QString & );

	private:
		GraphType      t;
		DataPointList *d;
		QString        gt;
		QString        xt;
		QString        yt;
};

#endif
