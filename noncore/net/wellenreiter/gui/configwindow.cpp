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

#include "configwindow.h"
#include <qmap.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>

WellenreiterConfigWindow::WellenreiterConfigWindow( QWidget * parent, const char * name, WFlags f )
           :WellenreiterConfigBase( parent, name, true, f )
{
    _devicetype[ "cisco" ] = 1;
    _devicetype[ "wlan-ng" ] = 2;
    _devicetype[ "hostap" ] = 3;
    _devicetype[ "orinoco" ] = 4;
    _devicetype[ "<manual>" ] = 5;

    #ifdef Q_WS_X11 // We're on X11: adding an Ok-Button for the Dialog here
    QPushButton* okButton = new QPushButton( "ok", this );
    okButton->show();
    Layout5_2->addWidget( okButton ); //FIXME: rename this in configbase.ui
    connect( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    #endif
};

int WellenreiterConfigWindow::daemonDeviceType()
{
    QString name = deviceType->currentText();
    if ( _devicetype.contains( name ) )
    {
        return _devicetype[name];
    }
    else
    {
        return 0;
    }
};

int WellenreiterConfigWindow::daemonHopInterval()
{
    return hopInterval->cleanText().toInt();
}
