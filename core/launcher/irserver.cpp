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
#include "obexinterface.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qtopia/qlibrary.h>
#include <qtopia/qpeapplication.h>

/* QT */
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
    //odebug << "trying lib " << (path + (*it)) << "" << oendl;
	if ( trylib->queryInterface( IID_ObexInterface, (QUnknownInterface**)&obexIface ) == QS_OK ) {
	    lib = trylib;
        //odebug << "found obex lib" << oendl;
	    QString lang = getenv( "LANG" );
	    QTranslator * trans = new QTranslator(qApp);
	    QString type = (*it).left( (*it).find(".") );
	    QString tfn = QPEApplication::qpeDir()+"/i18n/"+lang+"/"+type+".qm";
        //odebug << "tr fpr obex: " << tfn << "" << oendl;
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
    odebug << "could not load IR plugin" << oendl;
}

IrServer::~IrServer()
{
    if ( obexIface )
	obexIface->release();
    delete lib;
}

