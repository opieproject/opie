#include <opie2/oapplication.h>
#include <opie2/oconfig.h>
#include <qpe/config.h>

using namespace Opie::Core;

int main( int argc, char** argv )
{
    OApplication* app = new OApplication( argc, argv, "MyConfigDemoApplication" );

    OConfigGroupSaver c1( app->config(), "MyGroup" );
    app->config()->writeEntry( "AnEntry", "InMyGroup" );
    {
        OConfigGroupSaver c2( c1.config(), "AnotherGroup" );
        app->config()->writeEntry( "AnEntry", "InAnotherGroup" );
    }   // closing the scope returns to the last group

    app->config()->writeEntry( "AnotherEntry", "InMyGroup" );

    // do more stuff ...

    // in this (special) case it is necessary to manually call OConfig::write() (see below)
    app->config()->write();

    // can't delete the app when using the OConfigGroupSaver on top level scope,
    // because the destructor of the OConfigGroupSaver needs an application object
    //delete app; // destructor deletes config which writes changes back to disk

    return 0;

}

//#include "moc/oconfigdemo.moc"
