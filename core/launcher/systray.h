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

#ifndef __SYSTRAY_H__
#define __SYSTRAY_H__

#include <qpe/taskbarappletinterface.h>

#include <qframe.h>
#include <qvaluelist.h>

class QHBoxLayout;
class QLibrary;

struct TaskbarApplet
{
#ifndef QT_NO_COMPONENT
    QLibrary *library;
#endif
    TaskbarAppletInterface *iface;
    QWidget *applet;
};

class SysTray : public QFrame {
    Q_OBJECT
public:
    SysTray( QWidget *parent );

    void loadApplets();

private:
    void positionApplet( const TaskbarApplet &a );

private:
    QHBoxLayout *layout;
    QValueList<TaskbarApplet> appletList;
};


#endif // __SYSTRAY_H__

