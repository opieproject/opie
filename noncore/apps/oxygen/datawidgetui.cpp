/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/
#include "oxygen.h"

#include <qpe/config.h>
#include "datawidgetui.h"
#include <qtable.h>
#include <qstring.h>
#include <qcombobox.h>

dataWidgetUI::dataWidgetUI() : dataWidget()
{
    connect ( dataCombo, SIGNAL( activated(int) ), this, SLOT( slotShowData(int) ) );
    Config test( "/home/opie/Settings/oxygendata", Config::File );
    
    //read in all names of the 118 elements
    for ( int i = 1 ; i < 119 ; i++ )
    {
        test.setGroup( QString::number( i ) );
        QString foo = test.readEntry( "Name" );
        dataCombo->insertItem( foo );
    }

    createTableLayout();
    slotShowData( 1 ); //this way we do always get data here

}

void dataWidgetUI::createTableLayout(){
    dataTable->horizontalHeader()->hide();
    dataTable->verticalHeader()->hide();
    dataTable->setTopMargin( 0 );
    dataTable->setLeftMargin( 0 );
    
    dataTable->setText( 0,0,"Weight:" );
    dataTable->setText( 1,0,"Block" );
    dataTable->setText( 2,0,"Group" );
    dataTable->setText( 3,0,"Electronegativity" );
    dataTable->setText( 4,0,"Atomic radius" );
    dataTable->setText( 5,0,"Ionizationenergie" );
    dataTable->setText( 6,0,"Density" );
    dataTable->setText( 7,0,"Boilingpoint" );
}


void dataWidgetUI::slotShowData(int number){
    Config test( "/home/opie/Settings/oxygendata", Config::File );
    test.setGroup( QString::number( number+1 ));
    QString weight = test.readEntry( "Weight" );
    dataTable->setText( 0,1,weight ); 
    QString block = test.readEntry( "Block" );
    dataTable->setText( 1,1,block ); 
    QString grp = test.readEntry( "Group" );
    dataTable->setText( 2,1,grp ); 
    QString en = test.readEntry( "EN" );
    dataTable->setText( 3,1,en ); 
    QString ar = test.readEntry( "AR" );
    dataTable->setText( 4,1,ar ) ; 
    QString ion = test.readEntry( "IE" );
    dataTable->setText( 5,1,ion ); 
    QString dens = test.readEntry( "Density" );
    dataTable->setText( 6,1,dens ); 
    QString bp = test.readEntry( "BP" );
    dataTable->setText( 7,1,bp ); 
}
