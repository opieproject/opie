#include <stdio.h>
#include <stdlib.h>


#include <qpe/qpeapplication.h>
#include <opie2/oapplicationfactory.h>

#include "pindlg.h"

int main(int argc, char *argv[]) {
    QPEApplication oapp(argc, argv);
    
    OpieTooth::PinDlg dlg;
    return oapp.exec();
}

