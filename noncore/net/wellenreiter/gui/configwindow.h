/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef WELLENREITERCONFIGWINDOW_H
#define WELLENREITERCONFIGWINDOW_H

#include "configbase.h"
#include <qmap.h>
#include <qcombobox.h>
#include <qstring.h>

class WellenreiterConfigWindow;

class WellenreiterConfigWindow : public WellenreiterConfigBase
{

  public:
    WellenreiterConfigWindow( QWidget * parent = 0, const char * name = "WellenreiterConfigWindow", WFlags f = 0 );
    int daemonDeviceType();
    int daemonHopInterval();
    const QString soundOnNetwork() const { return netSound->currentText(); };
    const QString soundOnBeacon() const { return beaconSound->currentText(); };
    static WellenreiterConfigWindow* instance() { return _instance; };

  protected:
    QMap<QString, int> _devicetype;
    static WellenreiterConfigWindow* _instance;

};

#endif

