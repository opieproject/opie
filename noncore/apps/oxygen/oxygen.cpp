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
#include <qtabwidget.h>
#include "calcdlg.h"
#include "calcdlgui.h"
#include "datawidgetui.h"
#include "psewidget.h"

Oxygen::Oxygen() : QMainWindow()
{
    PSEWidget *pse = new PSEWidget();
    calcDlgUI *CalcDlgUI = new calcDlgUI();
    dataWidgetUI *DataWidgetUI = new dataWidgetUI();
    
    this->setCaption( tr( "Oxygen" ) );
    
    QTabWidget *tabw = new QTabWidget( this , "qtab" );
    tabw->addTab( pse, tr( "PSE" ));
    tabw->addTab( DataWidgetUI , tr( "Data" ) );
    tabw->addTab( CalcDlgUI, tr( "Calculations" ) );
     setCentralWidget( tabw );
}
