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
***********************************************************************/

#define QTOPIA_INTERNAL_PRELOADACCESS


#include <stdlib.h>

#include <qpainter.h>

#include <qtopia/qcopenvelope_qws.h>

#include "runningappbar.h"
#include "serverinterface.h"

RunningAppBar::RunningAppBar(QWidget* parent)
  : QFrame(parent), selectedAppIndex(-1)
{
    QCopChannel* channel = new QCopChannel( "QPE/System", this );
    connect( channel, SIGNAL(received(const QCString&,const QByteArray&)),
	     this, SLOT(received(const QCString&,const QByteArray&)) );

    spacing = AppLnk::smallIconSize()+3;
}

RunningAppBar::~RunningAppBar()
{
}

void RunningAppBar::received(const QCString& msg, const QByteArray& data) {
    // Since fast apps appear and disappear without disconnecting from their
    // channel we need to watch for the showing/hiding events and update according.
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "fastAppShowing(QString)") {
	QString appName;
	stream >> appName;
	//    qDebug("fastAppShowing %s", appName.data() );
	const AppLnk* f = ServerInterface::appLnks().findExec(appName);
	if ( f ) addTask(*f);
    } else if ( msg == "fastAppHiding(QString)") {
	QString appName;
	stream >> appName;
	const AppLnk* f = ServerInterface::appLnks().findExec(appName);
	if ( f ) removeTask(*f);
    }
}

void RunningAppBar::addTask(const AppLnk& appLnk) {
    qDebug("Added %s to app list.", appLnk.name().latin1());
    AppLnk* newApp = new AppLnk(appLnk);
    newApp->setExec(appLnk.exec());
    appList.prepend(newApp);
    update();
}

void RunningAppBar::removeTask(const AppLnk& appLnk) {
  unsigned int i = 0;
  for (; i < appList.count() ; i++) {
    AppLnk* target = appList.at(i);
    if (target->exec() == appLnk.exec()) {
       qDebug("Removing %s from app list.", appLnk.name().latin1());
      appList.remove();
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
  selectedAppIndex = 0;
  int x=0;
  QListIterator<AppLnk> it( appList );
  for ( ; it.current(); ++it,++selectedAppIndex,x+=spacing ) {
    if ( x + spacing <= width() ) {
      if ( e->x() >= x && e->x() < x+spacing ) {
	if ( selectedAppIndex < (int)appList.count() ) {
	  repaint(FALSE);
	  return;
	}
      }
    } else {
      break;
    }
  }
  selectedAppIndex = -1;
  repaint( FALSE );
}

void RunningAppBar::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == QMouseEvent::RightButton)
	return;
    if ( selectedAppIndex >= 0 ) {
	QString app = appList.at(selectedAppIndex)->exec();
	QCopEnvelope e("QPE/System", "raise(QString)");
	e << app;
	selectedAppIndex = -1;
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

    p.fillRect( 0, 0, width(), height(), colorGroup().background() );

    QListIterator<AppLnk> it(appList);

    for (; it.current(); i++, ++it ) {
      if ( x + spacing <= width() ) {
	curApp = it.current();
        qWarning("Drawing %s", curApp->name().latin1() );
 	if ( (int)i == selectedAppIndex )
 	  p.fillRect( x, y, spacing, curApp->pixmap().height()+1, colorGroup().highlight() );
  	else
	  p.eraseRect( x, y, spacing, curApp->pixmap().height()+1 );
	p.drawPixmap( x, y, curApp->pixmap() );
	x += spacing;
      }
    }
}

QSize RunningAppBar::sizeHint() const
{
    return QSize( frameWidth(), AppLnk::smallIconSize()+frameWidth()*2+3 );
}

void RunningAppBar::applicationLaunched(const QString &appName)
{
    qDebug("desktop:: app: %s launched with pid ", appName.data() );
    const AppLnk* newGuy = ServerInterface::appLnks().findExec(appName);
    if ( newGuy && !newGuy->isPreloaded() ) {
	addTask( *newGuy );
    }
}

void RunningAppBar::applicationTerminated(const QString &app)
{
    const AppLnk* gone = ServerInterface::appLnks().findExec(app);
    if ( gone ) {
	removeTask(*gone);
    }
}



