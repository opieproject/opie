#include <opie2/oprocess.h>
#include <stdio.h>

using namespace Opie::Core;

int main( int argc, char** argv )
{
    printf( "my own PID seems to be '%d'\n", OProcess::processPID( "oprocessdemo" ) );
    printf( "the PID of process 'Mickey' seems to be '%d'\n\n", OProcess::processPID( "Mickey" ) );

    return 0;
}

