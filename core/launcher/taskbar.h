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

#ifndef TASKBAR_H
#define TASKBAR_H

#include <qhbox.h>
#include "serverinterface.h"
#include "startmenu.h"

class QLabel;
class QTimer;
class InputMethods;
class Wait;
class SysTray;
class RunningAppBar;
class QWidgetStack;
class QTimer;
class QLabel;
class LockKeyState;
class AppLnkSet;

class TaskBar : public QHBox {
    Q_OBJECT
public:
    TaskBar();
    ~TaskBar();

    void launchStartMenu() { if (sm) sm->launch(); }
    void refreshStartMenu() { if (sm) sm->refreshMenu(); }
    void setApplicationState( const QString &name, ServerInterface::ApplicationState state );

signals:
    void tabSelected(const QString&);

public slots:
    void startWait();
    void stopWait(const QString&);
    void stopWait();

    void clearStatusBar();
    void toggleNumLockState();
    void toggleCapsLockState();
    void toggleSymbolInput();
    void calcMaxWindowRect();

protected:
    void resizeEvent( QResizeEvent * );
    void styleChange( QStyle & );
    void setStatusMessage( const QString &text );
    
private slots:
    void receive( const QCString &msg, const QByteArray &data );

private:
    QTimer *waitTimer;
    Wait *waitIcon;
    InputMethods *inputMethods;
    SysTray *sysTray;
    RunningAppBar* runningAppBar;
    QWidgetStack *stack;
    QTimer *clearer;
    QLabel *label;
    LockKeyState* lockState;
    StartMenu *sm;
    bool resizeRunningApp;
};


#endif // TASKBAR_H
