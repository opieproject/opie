
#include <qpe/qpeapplication.h>
#include "sshkeys.h"

int main(int argc, char *argv[])
{
    QPEApplication a(argc, argv);
    SSHKeysApp app;

    a.showMainWidget(&app);

    return a.exec();
}

