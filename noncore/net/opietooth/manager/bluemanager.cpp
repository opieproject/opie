/*
 * today.cpp : main class
 *
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

#include "bluemanager.h"

#include <qpe/timestring.h>
#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
//#include <qpe/qprocess.h>
#include <qpe/resource.h>
#include <qpe/contact.h>
#include <qpe/global.h>
#include <qpe/qpeapplication.h>

#include <qdir.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qtextstream.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qpixmap.h>
//#include <qfileinfo.h>
#include <qlayout.h>
#include <qtl.h>


//#include <iostream.h>
#include <unistd.h>
#include <stdlib.h>


/*
 *  Constructs a Example which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
BlueManager::BlueManager( QWidget* parent,  const char* name, WFlags fl )
  : BluetoothBase( parent, name, fl ) {


  draw();
}


/*
 * Repaint method. Reread all fields.
 */
void BlueManager::draw() {

  QTimer::singleShot( 20*1000, this, SLOT(draw() ) );
}


void BlueManager::init() {

}

BlueManager::~BlueManager() {
}




