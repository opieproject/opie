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
#include "zcameraio.h"

#include <qvbox.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include <qpe/resource.h>
#include <opie/ofiledialog.h>

CameraMainWindow::CameraMainWindow( QWidget * parent, const char * name, WFlags f )
           :QMainWindow( parent, name, f )
{
    QVBox* v = new QVBox( this );
    l = new QLabel( v );
    l->setFixedSize( QSize( 240, 160 ) );
    QPushButton* p = new QPushButton( "Snapshot", v );
    connect( p, SIGNAL( clicked() ), this, SLOT( clickedSnapShot() ) );
    v->show();
    l->show();
    p->show();
    setCentralWidget( v );

};


CameraMainWindow::~CameraMainWindow()
{
}


void CameraMainWindow::clickedSnapShot()
{
    QImage i;
    QPixmap p;
    if ( ZCameraIO::instance()->snapshot( &i ) )
    {
        p.convertFromImage( i );
        l->setPixmap( p );
    }
}

