#include <qpe/qpeapplication.h>

#include "senderui.h"


int main( int argc, char* argv[] ){
QPEApplication app(argc,argv );

SenderUI ui;
app.showMainWidget(&ui);

return app.exec();
}