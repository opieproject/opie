/**********************************************************************
** Copyright (C) 2000-2003 Trolltech AS.  All rights reserved.
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

#ifndef SERVERAPP_H
#define SERVERAPP_H

#include <qtopia/qpeapplication.h>

#include <qwidget.h>
#ifdef QWS
#include <qwindowsystem_qws.h>
#endif

#include "shutdownimpl.h"

class PowerStatus;
class DesktopPowerAlerter;
class DeviceButton;

class KeyFilter : public QObject, public QWSServer::KeyboardFilter {
    Q_OBJECT
public:
    KeyFilter(QObject* parent);
    bool filter(int unicode, int keycode, int modifiers, bool press,
                      bool autoRepeat);

protected:
    void timerEvent(QTimerEvent*);

signals:
    void launch();
    void power();
    void backlight();
    void symbol();
    void numLockStateToggle();
    void capsLockStateToggle();
    void activate(const DeviceButton*,bool);

private:
    int held_tid;
    const DeviceButton* heldButton;
};

class ServerApplication : public QPEApplication
{
    Q_OBJECT
public:
    ServerApplication( int& argc, char **argv, Type t );
    ~ServerApplication();

    static bool doRestart;
    static bool allowRestart;
    static bool screenLocked();
    static void login(bool at_poweron);

    void restart();

signals:
    void home();
    void launch();
    void power();
    void backlight();
    void symbol();
    void numLockStateToggle();
    void capsLockStateToggle();
    void prepareForRestart();
    void activate(const DeviceButton*,bool);

protected:
#ifdef Q_WS_QWS
    bool qwsEventFilter( QWSEvent * );
#endif
    void shutdown();
    void checkMemory();
    bool recoverMemory();
    void keyClick(int keycode, bool press, bool repeat);
    void screenClick(bool press);

protected slots:
    void shutdown(ShutdownImpl::Type);
    void psTimeout();
    void showSafeMode();
    void clearSafeMode();
    void togglePower();
    void toggleLight();
    void rereadVolumes();

private:
    DesktopPowerAlerter *pa;
    PowerStatus *ps;
    bool keyclick;
    bool touchclick;

    friend class KeyFilter;
};


#endif // SERVERAPP_H

