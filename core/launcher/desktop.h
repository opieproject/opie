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

#ifndef __DESKTOP_H__
#define __DESKTOP_H__


#include "shutdownimpl.h"

#include <qpe/qpeapplication.h>
#include <opie/odevicebutton.h>

#include <qwidget.h>
#include <qdatetime.h>

class Background;
class Launcher;
class TaskBar;
class PowerStatus;
class QCopBridge;
class TransferServer;
class DesktopPowerAlerter;
class PackageSlave;
class OpieScreenSaver;

class DesktopApplication : public QPEApplication
{
	Q_OBJECT
public:
	DesktopApplication( int& argc, char **argv, Type t );
	~DesktopApplication();

	static void switchLCD ( bool on ); // only for togglePower in Desktop

	static void soundAlarm(); // only because QCop soundAlarm() is defined in QPE/TaskBar

signals:
	void menu();
	void home();
	void launch();
	void backlight();
	void power();
	void symbol();
	void numLockStateToggle();
	void capsLockStateToggle();
	void prepareForRestart();

protected:
#ifdef Q_WS_QWS

	bool qwsEventFilter( QWSEvent * );
#endif

	void shutdown();
	void restart();

public slots:
	virtual void systemMessage ( const QCString &msg, const QByteArray &data );
	virtual void launcherMessage ( const QCString &msg, const QByteArray &data );
	void rereadVolumes();

protected slots:
	void shutdown ( ShutdownImpl::Type );
	void apmTimeout ( );
	void sendHeldAction ( );

protected:
	virtual bool eventFilter ( QObject *o, QEvent *e );
	void checkButtonAction ( const Opie::ODeviceButton *db, int keycode, bool press, bool autoRepeat );
	
private:
	static DesktopApplication *me ( );
	
private:
	void reloadPowerWarnSettings();
	DesktopPowerAlerter *pa;
	PowerStatus *m_ps, *m_ps_last;
	QTimer *cardSendTimer;
	QCopChannel *channel;
	OpieScreenSaver *m_screensaver;
	QTimer * m_apm_timer;
	int m_powerVeryLow;
	int m_powerCritical;
	int m_currentPowerLevel;

	const Opie::ODeviceButton *m_last_button;
	QTimer *m_button_timer;	

	bool m_keyclick_sound  : 1;
	bool m_screentap_sound : 1;
	bool m_alarm_sound     : 1;
};


class Desktop : public QWidget
{
	Q_OBJECT
public:
	Desktop();
	~Desktop();

	static bool screenLocked();

	void show();
	void checkMemory();

public slots:
	void execAutoStart();
	void togglePower();
	void toggleLight();
	void toggleNumLockState();
	void toggleCapsLockState();
	void toggleSymbolInput();
	void terminateServers();

protected:
	void executeOrModify( const QString& appLnkFile );
	void styleChange( QStyle & );
	void timerEvent( QTimerEvent *e );

	QWidget *bg;
	Launcher *launcher;
	TaskBar *tb;

private:
	void startTransferServer();
	bool recoverMemory();

	QCopBridge *qcopBridge;
	TransferServer *transferServer;
	PackageSlave *packageSlave;

	QDateTime suspendTime;
};


#endif // __DESKTOP_H__

