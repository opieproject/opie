/*
 * main.cpp
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

#include "today.h"
#include <qpe/qpeapplication.h>

QPEApplication *todayApp;

int main( int argc, char ** argv ) 
{
  QPEApplication a(argc, argv);
  todayApp=&a;

  Today t;
  
  t.setCaption( Today::tr("Today") );
  a.showMainWidget(&t);
  
  return a.exec();
}
