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
#include "previewwidget.h"
#include "zcameraio.h"

#include <qvbox.h>
#include <qcombobox.h>
#include <qcursor.h>
#include <qdatastream.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qdirectpainter_qws.h>
#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>
#include <opie/ofiledialog.h>

#include <opie2/odebug.h>

#include <assert.h>

CameraMainWindow::CameraMainWindow( QWidget * parent, const char * name, WFlags f )
           :QMainWindow( parent, name, f )
{
    if ( !ZCameraIO::instance()->isOpen() )
    {
        QVBox* v = new QVBox( this );
        v->setMargin( 10 );
        QLabel* l1 = new QLabel( v );
        l1->setPixmap( Resource::loadPixmap( "camera/error" ) );
        QLabel* l2 = new QLabel( v );
        l2->setText( "<b>Sorry. could not detect your camera :-(</b><p>"
        "* Is the sharpzdc_cs module loaded ?<br>"
        "* Is /dev/sharpzdc read/writable ?<p>" );
        connect( new QPushButton( "Exit", v ), SIGNAL( clicked() ), this, SLOT( close() ) );
        setCentralWidget( v );
        return;
    }

    _rotation = 270; //TODO: grab these from the actual settings

    preview = new PreviewWidget( this, "camera preview widget" );
    //setCentralWidget( preview ); <--- don't do this!
    preview->resize( QSize( 240, 288 ) );
    preview->show();

    // construct a System Channel to receive setRotation messages
    _sysChannel = new QCopChannel( "QPE/System", this );
    connect( _sysChannel, SIGNAL( received( const QCString&, const QByteArray& ) ),
             this, SLOT( systemMessage( const QCString&, const QByteArray& ) ) );

    connect( preview, SIGNAL( contextMenuRequested() ), this, SLOT( showContextMenu() ) );
};


CameraMainWindow::~CameraMainWindow()
{
}


void CameraMainWindow::systemMessage( const QCString& msg, const QByteArray& data )
{
    QDataStream stream( data, IO_ReadOnly );
    odebug << "received system message: " << msg << oendl;
    if ( msg == "setCurrentRotation(int)" )
    {
        stream >> _rotation;
        odebug << "received setCurrentRotation(" << _rotation << ")" << oendl;

        switch ( _rotation )
        {
            case 270: preview->resize( QSize( 240, 288 ) ); break;
            case 180: preview->resize( QSize( 320, 208 ) ); break;
            default: QMessageBox::warning( this, "opie-camera",
                "This rotation is not supported.\n"
                "Supported are 180° and 270°" );
        }
    }
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


void CameraMainWindow::showContextMenu()
{
    QPopupMenu m( this );
    m.insertItem( "Item 1" );
    m.insertItem( "Item 1" );
    m.insertItem( "Item 1" );
    m.insertItem( "Item 1" );
    m.exec( QCursor::pos() );
}

