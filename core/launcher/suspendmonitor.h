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
#ifndef SUSPEND_MONITOR_H
#define SUSPEND_MONITOR_H


#include <qobject.h>
#include <qvaluelist.h>


class TempScreenSaverMonitor : public QObject
{
    Q_OBJECT
public:
    TempScreenSaverMonitor(QObject *parent = 0, const char *name = 0);

    void setTempMode(int,int);
    void applicationTerminated(int);

signals:
    void forceSuspend();

protected:
    void timerEvent(QTimerEvent *);

private:
    bool removeOld(int);
    void updateAll();
    int timerValue();

private:
    QValueList<int> sStatus[3];
    int currentMode;
    int timerId;
};


#endif // SUSPEND_MONITOR_H

