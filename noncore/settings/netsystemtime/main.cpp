#include <stdio.h>
#include "ntp.h"
#include <qpe/qpeapplication.h>

int main( int argc, char ** argv )
{
  	printf("This is netsystemtime\n");
	  printf("$Id: main.cpp,v 1.3 2002-07-06 13:53:48 tille Exp $\n");

    QPEApplication a( argc, argv );

    Ntp mw;
    a.showMainWidget( &mw );

    return a.exec();
}
