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

#include <qpe/qpeapplication.h>
#include <qpe/qlibrary.h>
#include <qpe/config.h>

#include <qlayout.h>
#include <qdir.h>
#include <qtranslator.h>

#include "quicklauncher.h"
#include "systray.h"

#include <stdlib.h>

#ifdef SINGLE_APP
#include "clockappletimpl.h"
#endif

SysTray::SysTray( QWidget *parent ) : QFrame( parent ), layout(0)
{
    //setFrameStyle( QFrame::Panel | QFrame::Sunken );
    loadApplets();
}

void SysTray::loadApplets()
{
#ifndef SINGLE_APP
    QValueList<TaskbarApplet>::Iterator mit;
    for ( mit = appletList.begin(); mit != appletList.end(); ++mit ) {
	(*mit).iface->release();
	(*mit).library->unload();
	delete (*mit).library;
    }
    appletList.clear();
    if ( layout )
	delete layout;
    layout = new QHBoxLayout( this );

    QString path = QPEApplication::qpeDir() + "/plugins/applets";
    QDir dir( path, "lib*.so" );
    QStringList list = dir.entryList();
    QStringList::Iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
	TaskbarAppletInterface *iface = 0;
	QLibrary *lib = new QLibrary( path + "/" + *it );
	if ( lib->queryInterface( IID_TaskbarApplet, (QUnknownInterface**)&iface ) == QS_OK ) {
	    TaskbarApplet applet;
	    applet.library = lib;
	    applet.iface = iface;
	    applet.applet = applet.iface->applet( this );
	    positionApplet( applet );
	    QString lang = getenv( "LANG" );
	    QTranslator * trans = new QTranslator(qApp);
	    QString type = (*it).left( (*it).find(".") );
	    QString tfn = QPEApplication::qpeDir()+"/i18n/"+lang+"/"+type+".qm";
	    qDebug("tr fpr sysapplet: %s", tfn.latin1() );
	    if ( trans->load( tfn ))
		qApp->installTranslator( trans );
	    else
		delete trans;
	} else {
	    delete lib;
	}
    }
#else
    layout = new QHBoxLayout( this );
    TaskbarApplet applet;
    applet.iface = new ClockAppletImpl();
    applet.applet = applet.iface->applet( this );
    positionApplet( applet );
#endif
}

void SysTray::positionApplet( const TaskbarApplet &a )
{
    int p = 0;
    QValueList<TaskbarApplet>::Iterator it;
    for ( it = appletList.begin(); it != appletList.end(); ++it ) {
	if ( (*it).iface->position() > a.iface->position() )
	    break;
	p += 2;
    }

    appletList.insert( it, a );
    layout->insertWidget( p, a.applet );
    layout->insertSpacing( p, 1 );
}

