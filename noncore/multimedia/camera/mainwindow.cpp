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

#include "mainwindow.h"
#include "mainwindowbase.h"
#include "zcameraio.h"

#include <qvbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qdirectpainter_qws.h>
#include <qpe/resource.h>
#include <opie/ofiledialog.h>

#include <assert.h>

CameraMainWindow::CameraMainWindow( QWidget * parent, const char * name, WFlags f )
           :QMainWindow( parent, name, f )
{
    mw = new MainWindowBase( this, "main widget" );
    ZCameraIO::instance()->setCaptureFrame( 240, 160, 256 );
    setCentralWidget( mw );
    mw->show();

    connect( mw->zoom, SIGNAL( activated( int ) ), this, SLOT( changeZoom(int) ) );
};


CameraMainWindow::~CameraMainWindow()
{
}


void CameraMainWindow::changeZoom( int zoom )
{
    int z;
    switch ( zoom )
    {
        case 0: z = 128; break;
        case 1: z = 256; break;
        case 2: z = 512; break;
        default: assert( 0 ); break;
    }

    ZCameraIO::instance()->setCaptureFrame( 240, 160, z );
}

