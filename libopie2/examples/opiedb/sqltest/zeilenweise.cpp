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
 driver->setUrl("/home/ich/zeilenweise");
 if ( driver->open() ) {
     qWarning("could open");
 }else
     qWarning("wasn't able to open");
 OSQLRawQuery raw2("BEGIN TRANSACTION");      
 OSQLRawQuery *raw = new OSQLRawQuery("create table todolist(uid,key,value)");
 OSQLResult res = driver->query( &raw2 );
 res = driver->query( raw );
 delete raw;
 for (int i = 0; i< 10000; i++ ) {
     int uid = i;
     OSQLRawQuery *raw;
     raw = new OSQLRawQuery("insert into todolist VALUES("+QString::number(uid)+",'Categories',"+"'-122324;-12132')");
     OSQLResult res = driver->query(raw );
     delete raw;

     raw = new OSQLRawQuery("insert into todolist VALUES("+QString::number(uid) +
                       ",'Completed',1)" );
     res = driver->query(raw );
     delete raw;

     raw = new OSQLRawQuery("insert into todolist VALUES("+
                            QString::number(uid)+",'Progress',100)" );
     res = driver->query( raw );
     delete raw;

     raw = new OSQLRawQuery("insert into todolist VALUES("+
                            QString::number(uid)+",'Summary',"+
                            "'Summary234-"+ QString::number(uid) + "')");
     res = driver->query( raw );
     delete raw;

     raw = new OSQLRawQuery("insert into todolist VALUES("+
                            QString::number(uid)+",'HasDate',1)");
     res = driver->query( raw );
     delete raw;

     raw = new OSQLRawQuery("insert into todolist VALUES("+
                            QString::number(uid)+",'DateDay',5)");
     res = driver->query( raw );
     delete raw;

     raw = new OSQLRawQuery("insert into todolist VALUES("+
                            QString::number(uid)+",'DateMonth',8)");
     res = driver->query( raw );
     delete raw;

     raw = new OSQLRawQuery("insert into todolist VALUES("+
                            QString::number(uid)+",'DateYear',2002)");
     res = driver->query( raw );
     delete raw;

     raw = new OSQLRawQuery("insert into todolist VALUES("+
                            QString::number(uid)+",'Priority',1)");
     res = driver->query( raw );
     delete raw;

     raw = new OSQLRawQuery("insert into todolist VALUES("+
                            QString::number(uid)+",'Description','"  +
                            QString::number(uid) + "Description\n12344')");
     res = driver->query( raw );
     delete raw;
 }
 OSQLRawQuery raw3("COMMIT");
 res = driver->query(&raw3 );
};
