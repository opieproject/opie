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
#include "psewidget.h"
#include "oxyframe.h"

PSEWidget::PSEWidget() : QWidget()
{
    maingrid = new QGridLayout(  18 , 10 , -1 , "maingridlayout" );
    OxyFrame *configobj = new OxyFrame(this);
}
