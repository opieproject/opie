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

#include "datawidgetui.h"
#include "dataTable.h"

#include <qpe/config.h>
#include <qstring.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qpe/qpeapplication.h>

dataWidgetUI::dataWidgetUI(const QStringList &list) : QWidget()
{
	names = list;
	
	this->setCaption( tr( "Chemical Data" ));
    QGridLayout *qgrid = new QGridLayout( this, 2,1 );
    
    dataCombo = new QComboBox( this );
    
    //read in all names of the 118 elements
	int i = 0;
    for ( QStringList::ConstIterator it =  names.begin() ; it != names.end() ; ++it,i++)
    {
        dataCombo->insertItem( QString::number( i+1 )+" - "+*it );
    }
    OxydataWidget *oxyDW = new OxydataWidget(this, "OxydataWidget_oxyDW", names);
    connect ( dataCombo, SIGNAL( activated(int) ), oxyDW, SLOT( setElement(int) ) );
    oxyDW->setElement( 0 );
    oxyDW->setLayout();

    qgrid->addWidget(  dataCombo, 0,0);
    qgrid->addWidget(  oxyDW , 1,0 );
}


