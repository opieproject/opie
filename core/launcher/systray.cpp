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

#include <qtopia/qpeapplication.h>
#include <qtopia/qlibrary.h>
#include <qtopia/config.h>
#include <qtopia/pluginloader.h>

#include <qlayout.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qtranslator.h>

#include "systray.h"

#include <stdlib.h>

#ifdef QT_NO_COMPONENTS
#include "../plugins/applets/clockapplet/clockappletimpl.h"
#endif

SysTray::SysTray( QWidget *parent ) : QFrame( parent ), layout(0), loader(0)
{
    //setFrameStyle( QFrame::Panel | QFrame::Sunken );
    loadApplets();
}

SysTray::~SysTray()
{
    clearApplets();
}

static int compareAppletPositions(const void *a, const void *b)
{
    const TaskbarApplet* aa = *(const TaskbarApplet**)a;
    const TaskbarApplet* ab = *(const TaskbarApplet**)b;
    int d = ab->iface->position() - aa->iface->position();
    if ( d ) return d;
    return QString::compare(ab->name,aa->name);
}

void SysTray::loadApplets()
{
    hide();
    clearApplets();
    addApplets();
}

void SysTray::clearApplets()
{
#ifndef QT_NO_COMPONENTS
    if ( loader ) {
	QValueList<TaskbarApplet*>::Iterator mit;
	for ( mit = appletList.begin(); mit != appletList.end(); ++mit ) {
	    loader->releaseInterface( (*mit)->iface );
	    delete (*mit);
	}
    }
#endif
    appletList.clear();
    if ( layout )
	delete layout;
    layout = new QHBoxLayout( this, 0, 1 );
    layout->setAutoAdd(TRUE);
    delete loader;
    loader = 0;
}

void SysTray::addApplets()
{
    hide();
    delete loader;
    loader = new PluginLoader( "applets" );
#ifndef QT_NO_COMPONENTS
    QStringList faulty;

    QStringList list = loader->list();
    TaskbarApplet **applets = new TaskbarApplet* [list.count()];
    QStringList::Iterator it;
    int napplets=0;
    for ( it = list.begin(); it != list.end(); ++it ) {
	TaskbarAppletInterface *iface = 0;
	if ( loader->queryInterface( *it, IID_TaskbarApplet, (QUnknownInterface**)&iface ) == QS_OK ) {
	    TaskbarApplet *applet = new TaskbarApplet;
	    applets[napplets++] = applet;
	    applet->iface = iface;
	    applet->name = *it;
	} else {
#ifndef Q_OS_WIN32
	    // Same as Taskbar settings uses
	    QString name = (*it).mid(3);
            int sep = name.find( ".so" );
#else
	    QString name = (*it);
            int sep = name.find( ".dll" );
#endif
            if ( sep > 0 )
                name.truncate( sep );
            sep = name.find( "applet" );
            if ( sep == (int)name.length() - 6 )
                name.truncate( sep );
            name[0] = name[0].upper();
	    faulty += name; 
	    // we don't do anything with faulty anymore -
	    // maybe we should.
	}
    }

    qsort(applets,napplets,sizeof(applets[0]),compareAppletPositions);
    while (napplets--) {
	TaskbarApplet *applet = applets[napplets];
	applet->applet = applet->iface->applet( this );
	if ( applet->applet->maximumSize().width() <= 1 )
	    applet->applet->hide();
	appletList.append(applet);
    }
    delete [] applets;
#else
    TaskbarApplet * const applet = new TaskbarApplet();
    applet->iface = new ClockAppletImpl();
    applet->applet = applet->iface->applet( this );
    appletList.append( applet );
#endif
    show();
}

