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
#ifndef BATTERY_H
#define BATTERY_H

#include <qwidget.h>
#include <qguardedptr.h>

class PowerStatus;
class BatteryStatus;
class QTimer;

class BatteryMeter : public QWidget
{
    Q_OBJECT
public:
    BatteryMeter( QWidget *parent = 0 );
    ~BatteryMeter();

    QSize sizeHint() const;

protected:
    void timerEvent( QTimerEvent * );
    void paintEvent( QPaintEvent* );
    void mouseReleaseEvent( QMouseEvent * );

protected slots:
    void chargeTimeout();

protected:
    QGuardedPtr<BatteryStatus> batteryView;
    PowerStatus *ps;
    QTimer *chargeTimer;
    int percent;
    bool charging;
};

#endif
