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


#include "irserver.h"

#include <qtopia/qlibrary.h>
#include <qtopia/qpeapplication.h>

#include <qtranslator.h>

#include "obexinterface.h"

#include <qdir.h>

IrServer::IrServer( QObject *parent, const char *name )
  : QObject( parent, name ), obexIface(0)
{
    lib = 0;
    obexIface = 0;
    QString path = QPEApplication::qpeDir() + "/plugins/obex/";
#ifdef Q_OS_MACX
    QDir dir( path, "lib*.dylib" );
#else
    QDir dir( path, "lib*.so" );
#endif /* Q_OS_MACX */
    QStringList list = dir.entryList();
    QStringList::Iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
	QLibrary *trylib = new QLibrary( path + *it );
	//qDebug("trying lib %s", (path + (*it)).latin1() );
	if ( trylib->queryInterface( IID_ObexInterface, (QUnknownInterface**)&obexIface ) == QS_OK ) {
	    lib = trylib;
	    //qDebug("found obex lib" );
	    QString lang = getenv( "LANG" );
	    QTranslator * trans = new QTranslator(qApp);
	    QString type = (*it).left( (*it).find(".") );
	    QString tfn = QPEApplication::qpeDir()+"/i18n/"+lang+"/"+type+".qm";
	    //qDebug("tr fpr obex: %s", tfn.latin1() );
	    if ( trans->load( tfn ))
		qApp->installTranslator( trans );
	    else
		delete trans;

	    break;
	} else {
	    delete lib;
	}
    }
    if ( !lib )
	qDebug("could not load IR plugin" );
}

IrServer::~IrServer()
{
    if ( obexIface )
	obexIface->release();
    delete lib;
}

