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
    this->setFrameStyle( QFrame::Panel );
    this->setLineWidth( 2 );
    this->setFrameShadow( QFrame::Raised );
    this->setBackgroundColor( QColor( cyan ) );
}

