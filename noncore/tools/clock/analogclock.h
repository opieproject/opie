/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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
#ifndef ANALOGCLOCK_H
#define ANALOGCLOCK_H

#include <qdatetime.h>
#include <qframe.h>

class AnalogClock : public QFrame
{
    Q_OBJECT
public:
    AnalogClock( QWidget *parent=0, const char *name=0 );

    QSizePolicy sizePolicy() const;

    void display( const QTime& time );

protected:
    void drawContents( QPainter *p );
    void drawHand( QPainter *p, QPoint, QPoint );
    void drawPointers ( QPainter *p, const QRect &r, const QColor &c, const QTime &t, const QTime *t2 = 0);

private:
    QPoint rotate( QPoint center, QPoint p, int angle );

    QTime currTime;
    QTime prevTime;
    bool clear;
};

#endif

