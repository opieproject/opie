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

#ifndef STYLUSNORMALIZER_H
#define STYLUSNORMALIZER_H

#include <qdatetime.h>
#include <qwidget.h>

class QTimer;

class _StylusEvent
{
public:
    _StylusEvent( const QPoint &pt = QPoint( 0, 0 ) );
    ~_StylusEvent();
    QPoint point( void ) const { return _pt; };
    QTime time( void ) const { return _t; };
    void setPoint( int x, int y) { _pt.setX( x ); _pt.setY( y ); };
    void setPoint( const QPoint &newPt ) { _pt = newPt; };
    void setTime( QTime newTime ) { _t = newTime; };

private:
    QPoint _pt;
    QTime _t;
};


class StylusNormalizer : public QWidget
{
    Q_OBJECT
public: 
    StylusNormalizer( QWidget *parent = 0, const char* name = 0 );
    ~StylusNormalizer();
    void start();
    void stop();
    void addEvent( const QPoint &pt );   // add a new point in

signals:
    void signalNewPoint( const QPoint &p );

private slots:
    void slotAveragePoint( void );  // return an averaged point

private:
    static const int SAMPLES = 10;
    _StylusEvent _ptList[SAMPLES];
    int _next;
    QTimer *_tExpire;
    bool bFirst;    // the first item added in...
};

#endif
