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

class OpieScreenSaver;
namespace Opie {
    class ODeviceButton;
}

struct QCopKeyRegister {
    QCopKeyRegister();
    QCopKeyRegister( int k, const QCString&, const QCString& );
    int keyCode()const;
    QCString channel()const;
    QCString message()const;
    inline bool send();

private:
    int m_keyCode;
    QCString m_channel, m_message;
};

typedef QMap<int, QCopKeyRegister> KeyRegisterList;

class KeyFilter : public QObject {
    Q_OBJECT
public:
    KeyFilter(QObject* parent);
    void registerKey( const QCopKeyRegister& );
    void unregisterKey( const QCopKeyRegister& );
    bool checkButtonAction( bool, int, int, int );



protected:
    void timerEvent(QTimerEvent*);

signals:
    void launch();
    void power();
    void backlight();
    void symbol();
    void numLockStateToggle();
    void capsLockStateToggle();
    void activate(const Opie::ODeviceButton*,bool);


private:
    bool keyRegistered( int key );
    int held_tid;
    const Opie::ODeviceButton* heldButton;
    KeyRegisterList m_keys;
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
    
    static bool isStarting();

    static void switchLCD ( bool on ); // only for togglePower in Desktop
    static void soundAlarm(); // only because QCop soundAlarm() is defined in QPE/TaskBar

    void restart();
    int exec();

signals:
    void menu();
    void home();
    void launch();
    void power();
    void backlight();
    void symbol();
    void numLockStateToggle();
    void capsLockStateToggle();
    void prepareForRestart();
    void activate(const Opie::ODeviceButton*,bool);

public slots:
    virtual void systemMessage( const QCString& msg, const QByteArray& );
    virtual void launcherMessage( const QCString& msg, const QByteArray& );
    void rereadVolumes();

protected:
    bool eventFilter( QObject*,  QEvent* );
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
    void apmTimeout();
    void showSafeMode();
    void clearSafeMode();
    void togglePower();
    void toggleLight();

private:
    static ServerApplication *me ();
    void reloadPowerWarnSettings();
    KeyFilter *kf;


private:
    DesktopPowerAlerter *pa;
    PowerStatus *m_ps,  *m_ps_last;
    OpieScreenSaver *m_screensaver;
    QTimer *m_apm_timer;
    QDateTime m_suspendTime;
    int m_powerVeryLow;
    int m_powerCritical;
    int m_currentPowerLevel;

    bool m_keyclick_sound  : 1;
    bool m_screentap_sound : 1;
    bool m_alarm_sound     : 1;
    static bool ms_is_starting;


    friend class KeyFilter;
};


#endif // SERVERAPP_H

