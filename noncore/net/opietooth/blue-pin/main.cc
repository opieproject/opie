#include <stdio.h>
#include <stdlib.h>


#include <qpe/qpeapplication.h>
#include <qpe/config.h>

#include "pindlg.h"


int main( int argc, char* argv[] )
{
    if (argc < 2 ) {
        printf("ERR\n");
        exit(0);
    }
    QCString dir(argv[1] );
    QCString bdaddr( argv[2] );
    QCString name;
    if ( argc > 3 ) {
        name = argv[3];
    }
    QPEApplication a(argc, argv );
    QString status;
    if (dir == "out" ) {
        status = QObject::tr("Outgoing connection to ");
    }else
        status = QObject::tr("Incoming connection from ");
    status += name;
    status += "<br>";
    status += "[" + bdaddr + "]";
    OpieTooth::PinDlg dlg( status, bdaddr );
    if ( dlg.exec() ) {
        printf("PIN:%s\n",  dlg.pin().stripWhiteSpace().latin1() );
    }else
        printf("ERR\n");
    return 0;
}

