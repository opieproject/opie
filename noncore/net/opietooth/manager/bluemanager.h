/*
 * bluemanager.h
 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef BLUEMANAGER_H
#define BLUEMANAGER_H

#include <qdatetime.h>
#include <qlist.h>

#include "bluetoothbase.h"

class QVBoxLayout;

class BlueManager : public BluetoothBase {
    Q_OBJECT

  public:
    BlueManager( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~BlueManager();

  private slots:
    void draw();
   private:
    void init();
  private slots:
    //  void channelReceived(const QCString &msg, const QByteArray & data);

 private:
};


#endif
