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
**********************************************************************
*/

#define QTOPIA_INTERNAL_PRELOADACCESS

// For "kill"
#include <sys/types.h>
#include <signal.h>

#include <qtimer.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include "qprocess.h"
#include <qpe/qpeapplication.h>
#include <qpe/applnk.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/global.h>
#include <qwindowsystem_qws.h>
#include "runningappbar.h"

RunningAppBar::RunningAppBar(QWidget* parent)
  : QFrame(parent), m_AppLnkSet(0L), m_SelectedAppIndex(-1)
{
    setBackgroundMode( PaletteBackground );

    m_AppLnkSet = new AppLnkSet( QPEApplication::qpeDir() + "apps" );

    connect(qwsServer, SIGNAL(newChannel(const QString&)), this, SLOT(newQcopChannel(const QString&)));
    connect(qwsServer, SIGNAL(removedChannel(const QString&)), this, SLOT(removedQcopChannel(const QString&)));
    QCopChannel* channel = new QCopChannel( "QPE/System", this );
    connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
	     this, SLOT(received(const QCString&, const QByteArray&)) );

    spacing = AppLnk::smallIconSize()+3;
}

RunningAppBar::~RunningAppBar() {
}

void RunningAppBar::newQcopChannel(const QString& channelName) {
  QString prefix("QPE/Application/");
  if (channelName.startsWith(prefix)) {
    QString appName = channelName.mid(prefix.length());
//     qDebug("App %s just connected!", appName.latin1());
    const AppLnk* newGuy = m_AppLnkSet->findExec(appName);
    if (newGuy && !newGuy->isPreloaded()) {
      addTask(*newGuy);
    }
  }
}

void RunningAppBar::removedQcopChannel(const QString& channelName) {
  QString prefix("QPE/Application/");
  if (channelName.startsWith(prefix)) {
    QString appName = channelName.mid(prefix.length());
    qDebug("App %s just disconnected!", appName.latin1());
    const AppLnk* newGuy = m_AppLnkSet->findExec(appName);
    if (newGuy) {
      removeTask(*newGuy);
    }
  }
}

void RunningAppBar::received(const QCString& msg, const QByteArray& data) {
  // Since fast apps appear and disappear without disconnecting from their
  // channel we need to watch for the showing/hiding events and update according.
  QDataStream stream( data, IO_ReadOnly );
  if ( msg == "fastAppShowing(QString)") {
    QString appName;
    stream >> appName;
    addTask(*m_AppLnkSet->findExec(appName));
  } else if ( msg == "fastAppHiding(QString)") {
    QString appName;
    stream >> appName;
    removeTask(*m_AppLnkSet->findExec(appName));
  }
}

void RunningAppBar::addTask(const AppLnk& appLnk) {
//   qDebug("Added %s to app list.", appLnk.name().latin1());
  AppLnk* newApp = new AppLnk(appLnk);
  newApp->setExec(appLnk.exec());
  m_AppList.prepend(newApp);
  update();
}

void RunningAppBar::removeTask(const AppLnk& appLnk) {
  unsigned int i = 0;
  for (; i < m_AppList.count() ; i++) {
    AppLnk* target = m_AppList.at(i);
    if (target->exec() == appLnk.exec()) {
        qDebug("Removing %s from app list.", appLnk.name().latin1());
        m_AppList.remove();
      delete target;
    }
  }
  update();
}

void RunningAppBar::mousePressEvent(QMouseEvent *e)
{
  // Find out if the user is clicking on an app icon...
  // If so, snag the index so when we repaint we show it
  // as highlighed.
  m_SelectedAppIndex = 0;
  int x=0;
  QListIterator<AppLnk> it( m_AppList );
  for ( ; it.current(); ++it,++m_SelectedAppIndex,x+=spacing ) {
    if ( x + spacing <= width() ) {
      if ( e->x() >= x && e->x() < x+spacing ) {
	if ( m_SelectedAppIndex < (int)m_AppList.count() ) {
	  repaint(FALSE);
	  return;
	}
      }
    } else {
      break;
    }
  }
  m_SelectedAppIndex = -1;
  repaint( FALSE );
}

void RunningAppBar::mouseReleaseEvent(QMouseEvent *e)
{
  if (e->button() == QMouseEvent::RightButton) {
    return;
  }
  if ( m_SelectedAppIndex >= 0 ) {
    QString channel = QString("QPE/Application/") + m_AppList.at(m_SelectedAppIndex)->exec();
    if (QCopChannel::isRegistered(channel.latin1())) {
//       qDebug("%s is running!", m_AppList.at(m_SelectedAppIndex)->exec().latin1());
      QCopEnvelope e(channel.latin1(), "raise()");
      // This class will delete itself after hearing from the app or the timer expiring
      (void)new AppMonitor(*m_AppList.at(m_SelectedAppIndex), *this);
    }
    else {
      removeTask(*m_AppList.at(m_SelectedAppIndex));
    }

    m_SelectedAppIndex = -1;
    update();
  }
}

void RunningAppBar::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    AppLnk *curApp;
    int x = 0;
    int y = (height() - AppLnk::smallIconSize()) / 2;
    int i = 0;

    //p.fillRect( 0, 0, width(), height(), colorGroup().background() );

    QListIterator<AppLnk> it(m_AppList);

    for (; it.current(); i++, ++it ) {
      if ( x + spacing <= width() ) {
	curApp = it.current();
 	if ( (int)i == m_SelectedAppIndex )
 	  p.fillRect( x, y, spacing, curApp->pixmap().height()+1, colorGroup().highlight() );
  	else
            // p.eraseRect( x, y, spacing, curApp->pixmap().height()+1 );
	p.drawPixmap( x, y, curApp->pixmap() );
	x += spacing;
      }
    }
}

QSize RunningAppBar::sizeHint() const
{
    return QSize( frameWidth(), AppLnk::smallIconSize()+frameWidth()*2+3 );
}

const int AppMonitor::RAISE_TIMEOUT_MS = 500;

AppMonitor::AppMonitor(const AppLnk& app, RunningAppBar& owner)
  : QObject(0L), m_Owner(owner), m_App(app), m_PsProc(0L), m_AppKillerBox(0L) {
  QCopChannel* channel = new QCopChannel( "QPE/System", this );
  connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
	   this, SLOT(received(const QCString&, const QByteArray&)) );
  connect(&m_Timer, SIGNAL(timeout()), this, SLOT(timerExpired()));
  m_Timer.start(RAISE_TIMEOUT_MS, TRUE);
}

AppMonitor::~AppMonitor() {
  if (m_AppKillerBox) {
    delete m_AppKillerBox;
    m_AppKillerBox = 0L;
  }
}

void AppMonitor::received(const QCString& msg, const QByteArray& data) {
  QDataStream stream( data, IO_ReadOnly );

  if (msg == "appRaised(QString)") {
    QString appName;
    stream >> appName;
    if (appName == m_App.exec()) {
      // qDebug("Got a heartbeat from %s", appName.latin1());
      m_Timer.stop();
      // Check to make sure we're not waiting on user input...
      if (m_AppKillerBox) {
	// If we are, we kill the dialog box, and the code waiting on the result
	// will clean us up (basically the user said "no").
	delete m_AppKillerBox;
	m_AppKillerBox = 0L;
      }
      else {
	// Ok, we're not waiting on user input, so clean us up now.
	// WE DELETE OURSELVES HERE!  Don't do anything else!!
	delete this;
      }
    }
  }
}

void AppMonitor::timerExpired() {
  //   qDebug("Checking in on %s", m_App.name().latin1());
  // We store this incase the application responds while we're
  // waiting for user input so we know not to delete ourselves.  This
  // will be cleaned up in the destructor.
  m_AppKillerBox = new QMessageBox(tr("Application Problem"),
				   tr("<p>%1 is not responding.</p>").arg(m_App.name()) +
				   tr("<p>Would you like to force the application to exit?</p>"),
				   QMessageBox::Warning, QMessageBox::Yes,
				   QMessageBox::No | QMessageBox::Default,
				   QMessageBox::NoButton);
  if (m_AppKillerBox->exec()  == QMessageBox::Yes) {
    // qDebug("Killing the app!!! Bwuhahahaha!");
    m_PsProc = new  QProcess(QString("ps"));
    m_PsProc->addArgument("h");
    m_PsProc->addArgument("-C");
    m_PsProc->addArgument(m_App.exec());
    m_PsProc->addArgument("-o");
    m_PsProc->addArgument("pid");
    connect(m_PsProc, SIGNAL(processExited()), this, SLOT(psProcFinished()));
    m_PsProc->start();
  }
  else {
    // qDebug("Wuss..");
    // WE DELETE OURSELVES HERE!  Don't do anything else!!
    delete this;
  }
}

void AppMonitor::psProcFinished() {
  QString pid = m_PsProc->readLineStdout();
  delete m_PsProc;
  m_PsProc = 0L;

  // qDebug("Killing app %s", pid.latin1());
  if (pid.isEmpty()) {
    // Hmm.. did the application bail before we got there?
    qDebug("AppMonitor: Tried to kill application %s but ps couldn't find it.", m_App.exec().latin1());
  }
  else {
    int success = kill(pid.toUInt(), SIGKILL);
    if (success == 0) {
      m_Owner.removeTask(m_App);
    }
    else {
      qWarning("Could not kill task %s", m_App.exec().latin1());
    }
  }

  // WE DELETE OURSELVES HERE!  Don't do anything else!!
  delete this;
}
