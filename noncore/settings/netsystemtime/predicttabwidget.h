/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 OPIE team <opie@handhelds.org?>
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

#ifndef PREDICTTABWIDGET_H
#define PREDICTTABWIDGET_H

#include <qdatetime.h>
#include <qstring.h>
#include <qwidget.h>

class QLabel;
class QTable;

class PredictTabWidget : public QWidget
{
	Q_OBJECT

public:
	PredictTabWidget( QWidget * = 0x0 );
	~PredictTabWidget();

	void setShiftPerSec( int );

private:
	QTable *tblLookups;
	QLabel *lblDrift;
	QLabel *lblDiff;
	QLabel *lblPredicted;

	float     _shiftPerSec;
	QString   tz;
	QDateTime predictedTime;

signals:
	void setTime( const QDateTime & );

public slots:
	void slotTZChanged( const QString & );
	void slotPredictTime();
	void slotSetPredictedTime();
};

#endif
