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
#ifndef ALARM_SERVER_H
#define ALARM_SERVER_H

#include <qstring.h>
#include <qdatetime.h>

class AlarmServer
{
public:
    static void addAlarm ( QDateTime when, const QCString& channel, const QCString& msg, int data=0);
    static void deleteAlarm (QDateTime when, const QCString& channel, const QCString& msg, int data=0);

private:
    friend int initApplication(int, char **);
    static void initialize();
};

#endif

