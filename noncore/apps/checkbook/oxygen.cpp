/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 *  ***********************************************************************/
#include "oxygen.h"

#include <qmenubar.h>
#include <qstatusbar.h>
#include <qpopupmenu.h>
#include <qlabel.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <qdir.h>
#include <qvbox.h>
#include "calcdlg.h"
#include "calcdlgui.h"

Oxygen::Oxygen()
  : QMainWindow()
{
    this->setCaption( "Oxygen" );
    vbox = new QVBox( this );
    QPushButton *setButton = new QPushButton( "Settings", vbox );
    connect ( setButton, SIGNAL( clicked() ), this, SLOT( slotSettings() ) );
    QPushButton *calcButton = new QPushButton( "Calculations", vbox );
    connect ( calcButton, SIGNAL( clicked() ), this, SLOT( slotCalculations() ) );
    QPushButton *pseButton = new QPushButton( "PSE", vbox );
    connect ( pseButton, SIGNAL( clicked() ), this, SLOT( slotPSE() ) );

    setCentralWidget( vbox );
}


void Oxygen::close()
{
  QApplication::exit();
}

//SLOTS

void Oxygen::slotCalculations(){ 
    calcDlgUI *CalcDlgUI = new calcDlgUI();
    CalcDlgUI->show();
};

void Oxygen::slotSettings(){ };
void Oxygen::slotPSE(){ };

