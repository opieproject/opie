#include "irserver.h"

#include <qpe/opielibrary.h>
#include <qpe/qpeapplication.h>

#include <qtranslator.h>
#include <stdlib.h>

#include "obexinterface.h"

#include <qdir.h>

IrServer::IrServer( QObject *parent, const char *name )
  : QObject( parent, name )
{
    lib = 0;
    QString path = QPEApplication::qpeDir() + "/plugins/obex/";
    QDir dir( path, "lib*.so" );
    QStringList list = dir.entryList();
    QStringList::Iterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
	ObexInterface *iface = 0;
	OpieLibrary *trylib = new OpieLibrary( path + *it );
	qDebug("trying lib %s", (path + (*it)).latin1() );
	if ( trylib->queryInterface( IID_ObexInterface, (QUnknownInterface**)&iface ) == QS_OK ) {
	    lib = trylib;
	    qDebug("found obex lib" );
	    QString lang = getenv( "LANG" );
	    QTranslator * trans = new QTranslator(qApp);
	    QString type = (*it).left( (*it).find(".") );
	    QString tfn = QPEApplication::qpeDir()+"/i18n/"+lang+"/"+type+".qm";
	    qDebug("tr fpr obex: %s", tfn.latin1() );
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
    delete lib;
}
