#include <qdir.h>

#include <qpe/qpeapplication.h>
#include "../osqlmanager.h"
#include "../osqlquery.h"
#include "../osqldriver.h"
#include "../osqlresult.h"

int main( int argc,  char* argv[] ) {

    QPEApplication app( argc, argv );
    OSQLManager man;
    man.registerPath( QDir::currentDirPath() );
    OSQLBackEnd::ValueList list = man.queryBackEnd();

    OSQLDriver *driver = man.standard();
    qWarning("testmain" + driver->id() );
    driver->setUrl("/home/ich/spaltenweise");
    if ( driver->open() ) {
        qWarning("could open");
    }else
        qWarning("wasn't able to open");
    OSQLRawQuery *raw = new OSQLRawQuery("create table todolist("
                                         "uid,categories,completed,"
                                         "progress,summary,HasDate,"
                                         "DateDay,DateMonth,DateYear,"
                                         "priority,description)" );

    OSQLResult res = driver->query( raw );
    delete raw;
    for (int i = 0; i< 10000; i++ ) {
        int uid = i;
        OSQLRawQuery raw("insert into todolist VALUES("+
                         QString::number(uid)+ ",'-122324;-12132',1,100,"+
                         "'Summary234-"+QString::number(uid)+"',1,5,8,2002,1,"+
                         "'Description\n12344')");
        OSQLResult res = driver->query( &raw );

    }
    return 0;
};
