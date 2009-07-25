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

#ifndef RUNNING_APP_BAR_H
#define RUNNING_APP_BAR_H

#include <qtopia/applnk.h>
#include <qtopia/global.h>

#include <qframe.h>
#include <qlist.h>
#include <qtimer.h>
#include <qmap.h>
#include <qguardedptr.h>

class AppLnkSet;
class QCString;
class QProcess;
class QMessageBox;
class TempScreenSaverMonitor;

class RunningAppBar : public QFrame
{
    Q_OBJECT

public:
    RunningAppBar(QWidget* parent);
    ~RunningAppBar();

    QSize sizeHint() const;

protected:
    void addTask(const AppLnk& appLnk);
    void removeTask(const AppLnk& appLnk);
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

public slots:
    void applicationLaunched(const QString &);
    void applicationTerminated(const QString &);

private slots:
    void received(const QCString& msg, const QByteArray& data);

private:
    const AppLnkSet *appLnkSet;
    QList<AppLnk> appList;
    int selectedAppIndex;
    int spacing;
};


#endif

