/*
                             This file is part of the OPIE Project
               =.            Copyright (c)  2002 Maximilian Reiss <harlekin@handhelds.org>
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
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
#ifndef __CALIBRATION_H__
#define __CALIBRATION_H__

#include <qwidget.h>

class Calibration : public QWidget {
	Q_OBJECT
	
public:
	Calibration ( QWidget *parent = 0, const char *name = 0, WFlags fl = 0 );
	virtual ~Calibration ( );
	
	QSize scale ( ) const;	
	int lineSteps ( ) const;
	int interval ( ) const;
	QPoint startPoint ( ) const;
	QPoint endPoint ( ) const;	

signals:
	void startPointChanged ( const QPoint & );
	void endPointChanged ( const QPoint & );

public slots:
	void setScale ( const QSize &s );
	void setLineSteps ( int step );
	void setInterval ( int iv );
	void setStartPoint ( const QPoint &p );
	void setEndPoint ( const QPoint &p );
	
protected:
	virtual void paintEvent ( QPaintEvent * );
	virtual void mousePressEvent ( QMouseEvent * );	
	virtual void mouseMoveEvent ( QMouseEvent * );	
	virtual void mouseReleaseEvent ( QMouseEvent * );	

	void checkPoints ( );
	
private:
	QSize m_scale;
	QPoint m_p [2];
	int m_dragged;
	int m_steps;
	int m_interval;
};

#endif


