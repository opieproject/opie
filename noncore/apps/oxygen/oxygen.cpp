/***************************************************************************
   application:             : Oxygen

   begin                    : September 2002
   copyright                : ( C ) 2002 by Carsten Niehaus
   email                    : cniehaus@handhelds.org
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/
#include "oxygen.h"

#include <qlabel.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qvbox.h>
#include "calcdlg.h"
#include "calcdlgui.h"
#include "datawidgetui.h"
#include "psewidget.h"

Oxygen::Oxygen()
  : QMainWindow()
{
    this->setCaption( tr( "Oxygen" ) );
    vbox = new QVBox( this );
    QPushButton *dataButton = new QPushButton( "Data", vbox );
    connect ( dataButton, SIGNAL( clicked() ), this, SLOT( slotData() ) );
    QPushButton *calcButton = new QPushButton( "Calculations", vbox );
    connect ( calcButton, SIGNAL( clicked() ), this, SLOT( slotCalculations() ) );
    QPushButton *pseButton = new QPushButton( "PSE", vbox );
    connect ( pseButton, SIGNAL( clicked() ), this, SLOT( slotPSE() ) );

    setCentralWidget( vbox );
}


void Oxygen::close() const
{
  QApplication::exit();
}

//SLOTS

void Oxygen::slotCalculations() const{ 
    calcDlgUI *CalcDlgUI = new calcDlgUI();
    CalcDlgUI->show();
};

void Oxygen::slotData() const{ 
    dataWidgetUI *DataWidgetUI = new dataWidgetUI();
    DataWidgetUI->showMaximized();
    DataWidgetUI->show();
};

void Oxygen::slotPSE() const{
    PSEWidget *pse = new PSEWidget();
    pse->showMaximized();
    pse->show();
};

