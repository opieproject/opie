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
#include <qstring.h>
#include <qcombobox.h>
#include <qlayout.h>

dataWidgetUI::dataWidgetUI() : QDialog()
{
    this->setCaption("Chemical Data");
    
    QVBoxLayout *vbox = new QVBoxLayout( this );
    
    dataCombo = new QComboBox( this );
    DataTable = new OxydataTable( 7,2, this );
    DataTable->setShowGrid( false );

    vbox->addWidget( dataCombo );
    vbox->addWidget( DataTable );
    
    DataTable->show();
    dataCombo->show();
    
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
    DataTable->horizontalHeader()->hide();
    DataTable->verticalHeader()->hide();
    DataTable->setTopMargin( 0 );
    DataTable->setLeftMargin( 0 );
    
    DataTable->setText( 0,0,"Weight:" );
    DataTable->setText( 1,0,"Block" );
    DataTable->setText( 2,0,"Group" );
    DataTable->setText( 3,0,"Electronegativity" );
    DataTable->setText( 4,0,"Atomic radius" );
    DataTable->setText( 5,0,"Ionizationenergie" );
    DataTable->setText( 6,0,"Density" );
    DataTable->setText( 7,0,"Boilingpoint" );
}


void dataWidgetUI::slotShowData(int number){
    Config test( "/home/opie/Settings/oxygendata", Config::File );
    test.setGroup( QString::number( number+1 ));
    QString weight = test.readEntry( "Weight" );
    DataTable->setText( 0,1,weight ); 
    QString block = test.readEntry( "Block" );
    DataTable->setText( 1,1,block ); 
    QString grp = test.readEntry( "Group" );
    DataTable->setText( 2,1,grp ); 
    QString en = test.readEntry( "EN" );
    DataTable->setText( 3,1,en ); 
    QString ar = test.readEntry( "AR" );
    DataTable->setText( 4,1,ar ) ; 
    QString ion = test.readEntry( "IE" );
    DataTable->setText( 5,1,ion ); 
    QString dens = test.readEntry( "Density" );
    DataTable->setText( 6,1,dens ); 
    QString bp = test.readEntry( "BP" );
    DataTable->setText( 7,1,bp ); 
}
