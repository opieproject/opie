
#include <qstring.h>
#include <qtopia/qcom.h>
#include <qtopia/qlibrary.h>
#include <qtopia/applicationinterface.h>
#include <qtopia/resource.h>

#include <opie/owait.h>

#include <qmetaobject.h>
#include <qmap.h>

namespace Opie {

    struct PluginLoader {
        PluginLoader( const char* ) {
        }

        QRESULT queryInterface( const QString& app, const QUuid&, QUnknownInterface** );
        void releaseInterface( QUnknownInterface* );
        QMap<QUnknownInterface*, QLibrary*> libs;
    };

    /*
     * We can skip installing a Translator here because Opies QPEApplication
     * will do that in initApp for us as well
     */
    QRESULT PluginLoader::queryInterface( const QString& libFile, const QUuid& uuid, QUnknownInterface** iface ) {
        QRESULT res = QS_FALSE;
        *iface = 0;

        // Below lines from TT then mine again
        QString name = libFile;
        if ( libFile.findRev(".so") == (int)libFile.length()-3 ) {
            name = libFile.left( libFile.length()-3 );
            if ( name.find( "lib" ) == 0 )
                name = name.mid( 3 );
        }
        QString path = QPEApplication::qpeDir() + "/plugins/application/lib"+name+".so";

        QLibrary *lib = new QLibrary( path );
        if ( lib->queryInterface( uuid, iface ) == QS_OK && iface ) {
            libs.insert( *iface, lib );
            res = QS_OK;
        }

        return res;
    }

    void PluginLoader::releaseInterface( QUnknownInterface* iface ) {
        if ( libs.contains( iface ) ) {
            iface->release();
            delete libs[iface];
            libs.remove( iface ); // we only handle pointers so even if the object is not valid the address-space is
        }
    }

}

/* small hack ;) */
using namespace Opie;
