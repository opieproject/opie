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

#include "graphinfo.h"

GraphInfo::GraphInfo( GraphType type, DataPointList *data, const QString &title,
					  const QString &xtitle, const QString &ytitle )
{
	t = type;
	d = data;
	gt = title;
	xt = xtitle;
	yt = ytitle;
}

GraphInfo::~GraphInfo()
{
	if ( d )
	{
		for ( DataPointInfo *data = d->first(); data; data = d->next() )
		{
			delete data;
		}
	}
}

GraphInfo::GraphType GraphInfo::graphType()
{
	return t;
}

void GraphInfo::setGraphType( GraphType type )
{
	t = type;
}

DataPointList *GraphInfo::dataPoints()
{
	return d;
}

void GraphInfo::setDataPoints( DataPointList *data )
{
	d = data;
}

DataPointInfo *GraphInfo::firstDataPoint()
{
	return( d->first() );
}

DataPointInfo *GraphInfo::nextDataPoint()
{
	return( d->next() );
}

int GraphInfo::numberDataPoints()
{
	return( d->count() );
}

float GraphInfo::maxValue()
{
	float max = 0.0;
	for ( DataPointInfo *data = d->first(); data; data = d->next() )
	{
		if ( data->value() > max )
		{
			max = data->value();
		}
	}
	return max;
}

float GraphInfo::totalValue()
{
	float sum = 0.0;
	for ( DataPointInfo *data = d->first(); data; data = d->next() )
	{
		sum += data->value();
	}
	return sum;
}

void GraphInfo::setGraphTitle( const QString &title )
{
	gt = title;
}

void GraphInfo::setXAxisTitle( const QString &xtitle )
{
	xt = xtitle;
}

void GraphInfo::setYAxisTitle( const QString &ytitle )
{
	yt = ytitle;
}
