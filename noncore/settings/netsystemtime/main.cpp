#include <stdio.h>
#include "ntp.h"
#include <qpe/qpeapplication.h>

int main( int argc, char ** argv )
{
  	printf("This is netsystemtime ");
	  printf("$Id: main.cpp,v 1.4 2002-11-16 16:43:02 tille Exp $\n");

    QPEApplication a( argc, argv );

    Ntp mw;
    a.showMainWidget( &mw );

    return a.exec();
}
