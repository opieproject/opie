
#include <stdlib.h>
#include <stdio.h>

#include "xinelib.h"

int main( int argc,  char *argv[] ) {
    printf("FixME\n");
    //return 0;
    XINE::Lib lib;
    QString str = QString::fromLatin1( argv[1] );
    lib.play( str );
    for (;;);
    return 0;
}
