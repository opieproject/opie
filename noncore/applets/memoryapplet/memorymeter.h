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
#ifndef MEMORY_H
#define MEMORY_H

#include <qwidget.h>

class MemoryStatus;
class QTimer;

class MemoryMeter : public QWidget
{
    Q_OBJECT
public:
    MemoryMeter( QWidget *parent = 0 );
    ~MemoryMeter();
    static int position();

    QSize sizeHint() const;
    MemoryStatus* memoryView;

protected:
    void timerEvent( QTimerEvent * );
    void paintEvent( QPaintEvent* );
    void mousePressEvent( QMouseEvent * );

protected slots:
    void usageTimeout();

protected:
    QTimer *usageTimer;
    int percent, low, critical;
    QSize bvsz;
    bool updateMemoryViewGeometry();
};

#endif
