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
 *
 ***************************************************************************/

#include <qpe/qpeapplication.h>
#include <qvbox.h>

#include "calcdisplay.h"
#include "calckeypad.h"

#include <opie2/oapplicationfactory.h>

struct Layout : QVBox{
    static QString appName() { return QString::fromLatin1("euroconv"); }
    Layout(QWidget *p, const char* n, WFlags) 
	:QVBox(0,"fond" )
    {
        LCDDisplay *lcd = new LCDDisplay(this, "lcd");
	KeyPad *num = new KeyPad(lcd, this, "keypad");
    
        setCaption( QObject::tr("Euroconv") );
    };
    
};

using namespace Opie::Core;

OPIE_EXPORT_APP( OApplicationFactory<Layout> )

