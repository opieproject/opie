/*
 * You may copy, modify and or distribute without any limitation
 */
#include <opie2/odebug.h>
#include <opie2/opluginloader.h>

#include <opie2/todayplugininterface.h>

using Opie::Core::OPluginItem;
using Opie::Core::OGenericPluginLoader;

void debugLst( const OPluginItem::List& lst ) {
    for ( OPluginItem::List::ConstIterator it = lst.begin(); it != lst.end(); ++it )
        odebug << "Name " << (*it).name() << " " << (*it).path() << " " << (*it).position() << oendl;
}


int main( void ) {
    OGenericPluginLoader loader( "today", true );
    loader.setAutoDelete( true );

    odebug << "IS in Safe Mode" << loader.isInSafeMode() << oendl;

    OPluginItem::List  lst = loader.allAvailable( true );
    debugLst( lst );

    lst = loader.filtered( true );
    debugLst( lst );

    for ( OPluginItem::List::Iterator it = lst.begin(); it != lst.end(); ++it ) {
        QUnknownInterface* iface = loader.load( *it, IID_TodayPluginInterface );
    }

   /*
    * now it's autodelete so cleaned up for us
    */
}
