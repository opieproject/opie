/*
 * bluebase.cpp *
 * ---------------------
 *
 * begin       : Sun 10 17:20:00 CEST 2002
 * copyright   : (c) 2002 by Maximilian Reiﬂ
 * email       : max.reiss@gmx.de
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "bluebase.h"

#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qtabwidget.h>
#include <qscrollview.h>
#include <qvbox.h>
#include <qapplication.h>
#include <qcheckbox.h>

#include <qpe/resource.h>

BlueBase::BlueBase( QWidget* parent,  const char* name, WFlags fl )
    : BluetoothBase( parent, name, fl ) {


    QPalette pal = this->palette();
    QColor col = pal.color(QPalette::Active, QColorGroup::Background);
    pal.setColor(QPalette::Active, QColorGroup::Button, col);
    pal.setColor(QPalette::Inactive, QColorGroup::Button, col);
    pal.setColor(QPalette::Normal, QColorGroup::Button, col);
    pal.setColor(QPalette::Disabled, QColorGroup::Button, col);
    this->setPalette(pal);
}


BlueBase::~BlueBase(){
}


