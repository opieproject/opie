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

#ifndef __TASKBAR_H__
#define __TASKBAR_H__

#include <qhbox.h>

class QLabel;
class QTimer;
class InputMethods;
class Wait;
class SysTray;
//class MRUList;
class RunningAppBar;
class QWidgetStack;
class QTimer;
class QLabel;
class StartMenu;
class LockKeyState;

class TaskBar : public QHBox {
    Q_OBJECT
public:
    TaskBar();
    ~TaskBar();

    static QWidget *calibrate( bool );

    bool recoverMemory();

    StartMenu *startMenu() const { return sm; }
public slots:
    void startWait();
    void stopWait(const QString&);
    void stopWait();
    void clearStatusBar();
    void toggleNumLockState();
    void toggleCapsLockState();
    void toggleSymbolInput();

protected:
    void resizeEvent( QResizeEvent * );
    void styleChange( QStyle & );
    void setStatusMessage( const QString &text );

private slots:
    void calcMaxWindowRect();
    void receive( const QCString &msg, const QByteArray &data );

private:

    QTimer *waitTimer;
    Wait *waitIcon;
    InputMethods *inputMethods;
    SysTray *sysTray;
    //  MRUList *mru;
    RunningAppBar* runningAppBar;
    QWidgetStack *stack;
    QTimer *clearer;
    QLabel *label;
    LockKeyState* lockState;
    StartMenu *sm;
};


#endif // __TASKBAR_H__
