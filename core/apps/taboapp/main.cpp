/* OPIE */
#include <oappinterface.h>
#include <oappplugin.h>
#include <opie2/odebug.h>
#include <opie2/otabwidget.h>
#include <qpe/qpeapplication.h>
#include <qpe/qlibrary.h>
using namespace Opie::Core;
using namespace Opie::Ui;

/* QT */
#include <qdir.h>

int main( int argc, char **argv )
{
    QPEApplication a( argc, argv );

    OTabWidget *tabwidget = new OTabWidget(0, "tab widget");

    QString path = QPEApplication::qpeDir() + "/plugins/app";
    QDir dir( path, "lib*.so" );

    QStringList list = dir.entryList();
    QStringList::Iterator it;

    QInterfacePtr<OAppInterface> iface;
    for ( it = list.begin(); it != list.end(); ++it ) {
        QLibrary *lib = new QLibrary( path + "/" + *it );

    odebug << "querying: " << QString( path + "/" + *it ) << "" << oendl;
        if ( lib->queryInterface( IID_OAppInterface, (QUnknownInterface**)&iface ) == QS_OK ) {
        odebug << "accepted: " << QString( path + "/" + *it ) << "" << oendl;

        QList<QWidget> list = iface->widgets();
            QWidget *widget;
            for ( widget = list.first(); widget != 0; widget = list.next() )
                tabwidget->addTab(widget, QString(*it), QString(*it));

            QString lang = getenv( "LANG" );
        if (lang.isNull())
        lang = "en";
        QTranslator *trans = new QTranslator(qApp);
        QString type = (*it).left( (*it).find(".") );
        if (type.left(3) == "lib")
         type = type.mid(3);
        type = type.right( type.find("lib") );
        QString tfn = QPEApplication::qpeDir()+"/i18n/"+lang+"/"+type+".qm";
        if ( trans->load( tfn ))
        qApp->installTranslator( trans );
        else
        delete trans;
    }
    }

    a.showMainDocumentWidget(tabwidget);
    return a.exec();
}
