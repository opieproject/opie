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

dataWidgetUI::dataWidgetUI() : dataWidget()
{
    Config test( "oxygendata" );
    test.setGroup( "1" );
    
    if ( test.hasKey( "Name" ) )
    {
        qDebug ( "geht" );
    }else qDebug( "geht nicht" );

    QString foo = test.readEntry( "Name" );
    //test.writeEntry( "Name", "test123" );

    qDebug( "hier sollte was kommen" );
    qDebug( foo );
    
    dataTable->horizontalHeader()->hide();
    dataTable->verticalHeader()->hide();
    dataTable->setTopMargin( 0 );
    dataTable->setLeftMargin( 0 );
    dataTable->setText( 0,0,"Atomic Radius" );
    dataTable->setText( 0,1,"17 nm" );
    dataTable->setText( 1,0,"Elec. neg." );
    dataTable->setText( 1,1,"234 Joule" );
}
