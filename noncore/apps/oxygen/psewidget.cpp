/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/

#include <qpe/config.h>
#include <qlayout.h>
#include <qlist.h>
#include "psewidget.h"
#include "oxyframe.h"


Element::Element( const QString &name )
{
}

PSEWidget::PSEWidget() : QWidget()
{
    QList<Element> elements;
    elements.setAutoDelete( TRUE );
    elements.append( new Element( "1" ) );
    
    maingrid = new QGridLayout(  18 , 10 , -1 , "maingridlayout" );
    OxyFrame *configobj = new OxyFrame(this);
}
