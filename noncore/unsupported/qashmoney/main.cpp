#include <qpe/qpeapplication.h>
#include "qashmoney.h"

int main( int argc, char **argv )
  {
    QPEApplication a ( argc, argv );
    QashMoney qashmoney;
    a.showMainWidget ( &qashmoney );
    return a.exec();
  }



