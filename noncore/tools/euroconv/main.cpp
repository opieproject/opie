/****************************************************************************
 *
 * File:        main.cpp
 *
 * Description: main file for OPIE Euroconv aapp
 *              
 *               
 * Authors:     Eric Santonacci <Eric.Santonacci@talc.fr>
 *
 * Requirements:    Qt
 *
 * $Id: main.cpp,v 1.2.2.1 2003-06-01 13:45:32 zecke Exp $
 *
 ***************************************************************************/

#include <qpe/qpeapplication.h>
#include <qvbox.h>
//#include <qapplication.h>

#include "calcdisplay.h"
#include "calckeypad.h"



int main( int argc, char **argv ) {

    QPEApplication app( argc, argv );

    QVBox *layout = new QVBox(0, "fond");
    
    LCDDisplay lcd (layout, "lcd");
    KeyPad num(&lcd, layout, "keypad");
    app.setMainWidget(layout);
    layout->setCaption( QObject::tr("Euroconv") );
    layout->showMaximized ();
    layout->show();
    return app.exec();
}

