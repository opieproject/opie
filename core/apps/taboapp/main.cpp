#include <qdir.h>

#include <qpe/global.h>
#include <qpe/qpeapplication.h>
#include <qpe/qlibrary.h>
#include <oappinterface.h>
#include <oappplugin.h>

#include <opie/otabwidget.h>

int main( int argc, char **argv )
{
    QPEApplication a( argc, argv );
    int ret = 0;

    OTabWidget *tabwidget = new OTabWidget(0, "tab widget");

    QString path = QPEApplication::qpeDir() + "/plugins/app";
    QDir dir( path, "lib*.so" );

    QStringList list = dir.entryList();
    QStringList::Iterator it;

    QInterfacePtr<OAppInterface> iface;
    for ( it = list.begin(); it != list.end(); ++it ) {
        QLibrary *lib = new QLibrary( path + "/" + *it );

	qDebug( "querying: %s", QString( path + "/" + *it ).latin1() );
        if ( lib->queryInterface( IID_OAppInterface, (QUnknownInterface**)&iface ) == QS_OK ) {
	    qDebug( "accepted: %s", QString( path + "/" + *it ).latin1() );

	    QList<QWidget> list = iface->widgets();

            QWidget *widget;
            for ( widget = list.first(); widget != 0; widget = list.next() )
                tabwidget->addTab(widget, QString(*it), QString(*it));
	}
    }

    a.showMainDocumentWidget(tabwidget);
    return a.exec(); 
}
