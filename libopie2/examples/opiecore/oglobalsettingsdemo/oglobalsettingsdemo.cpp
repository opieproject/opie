#include <opie2/oglobalsettings.h>
#include <stdio.h>

using namespace Opie::Core;

int main( int argc, char** argv )
{
    printf( "current debugmode seems to be '%d'\n", OGlobalSettings::debugMode() );
    printf( "output information for this mode is '%s'\n", (const char*) OGlobalSettings::debugOutput() );

    return 0;

}

//#include "moc/oconfigdemo.moc"
