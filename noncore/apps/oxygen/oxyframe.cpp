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

#include <qpe/config.h>
#include "oxyframe.h"

OxyFrame::OxyFrame(QWidget *parent, const char *name) : QFrame(parent,name)
{
    N = name;
    this->setFrameStyle( QFrame::Box );
    this->setLineWidth( 0 );
    this->setMidLineWidth( 1 );
    this->setFrameShadow( QFrame::Sunken );
}

void OxyFrame::mousePressEvent (  QMouseEvent * e ){
    emit num( N );
};
