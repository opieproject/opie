#include <qdir.h>

#include <qpe/qpeapplication.h>
#include <opie2/osqlmanager.h>
#include <opie2/osqlquery.h>
#include <opie2/osqldriver.h>
#include <opie2/osqlresult.h>

using namespace Opie::DB;
int main( int argc,  char* argv[] ) {

QPEApplication app( argc, argv );
OSQLManager man;
man.registerPath( QDir::currentDirPath() );
OSQLBackEnd::ValueList list = man.queryBackEnd();

OSQLDriver *driver = man.standard();
 qWarning("testmain" + driver->id() );
 driver->setUrl("/home/ich/test2vhgytry");
 if ( driver->open() ) {
     qWarning("could open");
 }else
     qWarning("wasn't able to open");
 OSQLRawQuery raw("select * from t2" );
 OSQLResult res = driver->query( &raw );

 OSQLRawQuery raw2( "insert into t2 VALUES(ROWID,'Meine Mutter') ");
 res = driver->query(&raw2);

};
