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
#include "imageio.h"
#include "avi.h"

#include <qapplication.h>
#include <qaction.h>
#include <qvbox.h>
#include <qcombobox.h>
#include <qcursor.h>
#include <qdatastream.h>
#include <qfile.h>
#include <qimage.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qdirectpainter_qws.h>
#include <qpe/global.h>
#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>
#include <opie/ofiledialog.h>
#include <opie/odevice.h>
using namespace Opie;
#include <opie2/odebug.h>

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define CAPTUREFILE "/tmp/capture.dat"
#define OUTPUTFILE  "/tmp/output.avi"

CameraMainWindow::CameraMainWindow( QWidget * parent, const char * name, WFlags f )
           :QMainWindow( parent, name, f ),
           _rotation( 270 ), // FIXME: get this from current settings (ODevice?)
           _capturing( false ),
           _pics( 0 ), _videos( 0 )
{
    #ifdef QT_NO_DEBUG
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
    #endif

    init();

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

    connect( ZCameraIO::instance(), SIGNAL( shutterClicked() ), this, SLOT( shutterClicked() ) );

    updateCaption();

};


CameraMainWindow::~CameraMainWindow()
{
}


void CameraMainWindow::init()
{
    // TODO: Save this stuff in config
    flip = 'A'; // auto
    quality = 50;
    zoom = 1;
    captureX = 480;
    captureY = 640;
    captureFormat = "JPEG";

    resog = new QActionGroup( 0, "reso", true );
    resog->setToggleAction( true );
    new QAction( " 64 x  48", 0, 0, resog, 0, true );
    new QAction( "128 x  96", 0, 0, resog, 0, true );
    new QAction( "192 x 144", 0, 0, resog, 0, true );
    new QAction( "256 x 192", 0, 0, resog, 0, true );
    new QAction( "320 x 240", 0, 0, resog, 0, true );
    new QAction( "384 x 288", 0, 0, resog, 0, true );
    new QAction( "448 x 336", 0, 0, resog, 0, true );
    new QAction( "512 x 384", 0, 0, resog, 0, true );
    new QAction( "576 x 432", 0, 0, resog, 0, true );
    ( new QAction( "640 x 480", 0, 0, resog, 0, true ) )->setOn( true );

    qualityg = new QActionGroup( 0, "quality", true );
    qualityg->setToggleAction( true );
    new QAction( "  0 (minimal)", 0, 0, qualityg, 0, true );
    new QAction( " 25 (low)", 0, 0, qualityg, 0, true );
    ( new QAction( " 50 (good)", 0, 0, qualityg, 0, true ) )->setOn( true );
    new QAction( " 75 (better)", 0, 0, qualityg, 0, true );
    new QAction( "100 (best)", 0, 0, qualityg, 0, true );

    zoomg = new QActionGroup( 0, "zoom", true );
    zoomg->setToggleAction( true );
    ( new QAction( "x 1", 0, 0, zoomg, 0, true ) )->setOn( true );
    new QAction( "x 2", 0, 0, zoomg, 0, true );

    flipg = new QActionGroup( 0, "flip", true );
    flipg->setToggleAction( true );
    ( new QAction( "Auto (recommended)", 0, 0, flipg, 0, true ) )->setOn( true );
    new QAction( "0 (always off)", 0, 0, flipg, 0, true );
    new QAction( "X (always horizontal)", 0, 0, flipg, 0, true );
    new QAction( "Y (always vertical)", 0, 0, flipg, 0, true );
    new QAction( "* (always both)", 0, 0, flipg, 0, true );

    outputg = new QActionGroup( 0, "output", true );
    outputg->setToggleAction( true );
    ( new QAction( "JPEG", 0, 0, outputg, 0, true ) )->setOn( true );
    new QAction( "PNG", 0, 0, outputg, 0, true );
    new QAction( "BMP", 0, 0, outputg, 0, true );
    new QAction( "AVI", 0, 0, outputg, 0, true );

    connect( resog, SIGNAL( selected(QAction*) ), this, SLOT( resoMenuItemClicked(QAction*) ) );
    connect( qualityg, SIGNAL( selected(QAction*) ), this, SLOT( qualityMenuItemClicked(QAction*) ) );
    connect( zoomg, SIGNAL( selected(QAction*) ), this, SLOT( zoomMenuItemClicked(QAction*) ) );
    connect( flipg, SIGNAL( selected(QAction*) ), this, SLOT( flipMenuItemClicked(QAction*) ) );
    connect( outputg, SIGNAL( selected(QAction*) ), this, SLOT( outputMenuItemClicked(QAction*) ) );

}


void CameraMainWindow::systemMessage( const QCString& msg, const QByteArray& data )
{
    int _newrotation;

    QDataStream stream( data, IO_ReadOnly );
    odebug << "received system message: " << msg << oendl;
    if ( msg == "setCurrentRotation(int)" )
    {
        stream >> _newrotation;
        odebug << "received setCurrentRotation(" << _newrotation << ")" << oendl;

        switch ( _newrotation )
        {
            case 270: preview->resize( QSize( 240, 288 ) ); break;
            case 180: preview->resize( QSize( 320, 208 ) ); break;
            default: QMessageBox::warning( this, "opie-camera",
                "This rotation is not supported.\n"
                "Supported are 180° and 270°" );
        }

        if ( _newrotation != _rotation )
        {
            int tmp = captureX;
            captureX = captureY;
            captureY = tmp;
            _rotation = _newrotation;
        }

        updateCaption();

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
    QPopupMenu reso;
    reso.setCheckable( true );
    resog->addTo( &reso );

    QPopupMenu quality;
    quality.setCheckable( true );
    qualityg->addTo( &quality );

    QPopupMenu flip;
    flip.setCheckable( true );
    flipg->addTo( &flip );

    QPopupMenu zoom;
    zoom.setCheckable( true );
    zoomg->addTo( &zoom );

    QPopupMenu output;
    output.setCheckable( true );
    outputg->addTo( &output );

    QPopupMenu m( this );
    m.insertItem( "&Resolution", &reso );
    m.insertItem( "&Zoom", &zoom );
    m.insertItem( "&Flip", &flip );
    m.insertItem( "&Quality", &quality );
    m.insertItem( "&Output As", &output );

    #ifndef QT_NO_DEBUG
    m.insertItem( "&Debug!", this, SLOT( doSomething() ) );
    #endif

    m.exec( QCursor::pos() );
}


void CameraMainWindow::resoMenuItemClicked( QAction* a )
{
    switch ( _rotation )
    {
        case 270:
            captureY = a->text().left(3).toInt();
            captureX = a->text().right(3).toInt();
            break;
        case 180:
            captureX = a->text().left(3).toInt();
            captureY = a->text().right(3).toInt();
            break;
        default: QMessageBox::warning( this, "opie-camera",
            "This rotation is not supported.\n"
            "Supported are 180° and 270°" );
    }
    odebug << "Capture Resolution now: " << captureX << ", " << captureY << oendl;
    updateCaption();
}


void CameraMainWindow::qualityMenuItemClicked( QAction* a )
{
    quality = a->text().left(3).toInt();
    odebug << "Quality now: " << quality << oendl;
    updateCaption();
}


void CameraMainWindow::zoomMenuItemClicked( QAction* a )
{
    zoom = QString( a->text().at(2) ).toInt();
    odebug << "Zoom now: " << zoom << oendl;
    ZCameraIO::instance()->setZoom( zoom );
    updateCaption();
}


void CameraMainWindow::flipMenuItemClicked( QAction* a )
{
    flip = QString( a->text().at(0) );
    odebug << "Flip now: " << flip << oendl;
    if ( flip == "A" )
        ZCameraIO::instance()->setFlip( ZCameraIO::AUTOMATICFLIP );
    else if ( flip == "0" )
        ZCameraIO::instance()->setFlip( ZCameraIO::XNOFLIP | ZCameraIO::YNOFLIP );
    else if ( flip == "X" )
        ZCameraIO::instance()->setFlip( ZCameraIO::XFLIP );
    else if ( flip == "Y" )
        ZCameraIO::instance()->setFlip( ZCameraIO::YFLIP );
    else if ( flip == "*" )
        ZCameraIO::instance()->setFlip( ZCameraIO::XFLIP | ZCameraIO::YFLIP );

    updateCaption();
}


void CameraMainWindow::outputMenuItemClicked( QAction* a )
{
    captureFormat = a->text();
    odebug << "Output format now: " << captureFormat << oendl;
    updateCaption();
}


void CameraMainWindow::shutterClicked()
{
    if ( captureFormat != "AVI" ) // capture one photo per shutterClick
    {
        Global::statusMessage( "CAPTURING..." );
        qApp->processEvents();

        odebug << "Shutter has been pressed" << oendl;
        ODevice::inst()->touchSound();

        performCapture( captureFormat );
    }
    else // capture video! start with one shutter click and stop with the next
    {
        !_capturing ? startVideoCapture() : stopVideoCapture();
    }
}


void CameraMainWindow::performCapture( const QString& format )
{
    QString name;
    name.sprintf( "/tmp/image-%d_%d_%d_q%d.%s", _pics++, captureX, captureY, quality, (const char*) captureFormat.lower() );
    QImage i;
    ZCameraIO::instance()->captureFrame( captureX, captureY, zoom, &i );
    QImage im = i.convertDepth( 32 );
    bool result = im.save( name, format, quality );
    if ( !result )
    {
        oerr << "imageio-Problem while writing." << oendl;
        Global::statusMessage( "Error!" );
    }
    else
    {
        odebug << captureFormat << "-image has been successfully captured" << oendl;
        Global::statusMessage( "Ok." );
    }
}


void CameraMainWindow::startVideoCapture()
{
    //ODevice::inst()->touchSound();
    ODevice::inst()->setLedState( Led_Mail, Led_BlinkSlow );

    _capturefd = ::open( CAPTUREFILE, O_WRONLY | O_CREAT | O_TRUNC );
    if ( _capturefd == -1 )
    {
        owarn << "can't open capture file: " << strerror(errno) << oendl;
        return;
    }

    _capturebuf = new unsigned char[captureX*captureY*2];
    _capturing = true;
    _videopics = 0;
    _framerate = 0;
    updateCaption();
    _time.start();
    preview->setRefreshingRate( 1000 );
    startTimer( 100 ); // too fast but that is ok
}


void CameraMainWindow::timerEvent( QTimerEvent* )
{
    if ( !_capturing )
    {
        odebug << "timer event in CameraMainWindow without capturing video ?" << oendl;
        return;
    }

    odebug << "timer event during video - now capturing frame #" << _videopics+1 << oendl;

    ZCameraIO::instance()->captureFrame( captureX, captureY, zoom, _capturebuf );
    _videopics++;
    ::write( _capturefd, _capturebuf, captureX*captureY*2 );
    setCaption( QString().sprintf( "Capturing %dx%d @ %.2f fps %d",
        captureX, captureY, 1000.0 / (_time.elapsed()/_videopics), _videopics ) );
}


void CameraMainWindow::stopVideoCapture()
{
    killTimers();
    //ODevice::inst()->touchSound();
    ODevice::inst()->setLedState( Led_Mail, Led_Off );
    _capturing = false;
    updateCaption();
    ::close( _capturefd );
    _framerate = 1000.0 / (_time.elapsed()/_videopics);

    postProcessVideo( CAPTUREFILE, QString().sprintf( "/tmp/video-%d_%d_%d_q%d-%dfps.avi", _videos++, captureX, captureY, quality, _framerate ) );

    #ifndef QT_NO_DEBUG
    preview->setRefreshingRate( 1500 );
    #else
    preview->setRefreshingRate( 200 );
    #endif

    //delete[] _capturebuf; //FIXME: close memory leak
}

void CameraMainWindow::postProcessVideo( const QString& infile, const QString& outfile )
{
    preview->setRefreshingRate( 0 );

    /*
    unsigned char buf[153600];

    int fd = ::open( "/var/compile/opie/noncore/multimedia/camera/capture-320x240.dat", O_RDONLY );
    ::read( fd, &buf, 153600 );
    QImage i;
    bufferToImage( 240, 320, (unsigned char*) &buf, &i );
    QPixmap p;
    p.convertFromImage( i );
    preview->setPixmap( p );
    imageToFile( &i, "/tmp/tmpfile", "JPEG", 100 );
    return;
    */

    QDialog* fr = new QDialog( this, "splash", false, QWidget::WStyle_StaysOnTop ); //, false, QWidget::WStyle_NoBorder | QWidget::WStyle_Customize );
    fr->setCaption( "Please wait..." );
    QVBoxLayout* box = new QVBoxLayout( fr, 2, 2 );
    QProgressBar* bar = new QProgressBar( fr );
    bar->setCenterIndicator( true );
    bar->setTotalSteps( _videopics-1 );
    QLabel* label = new QLabel( "Post processing frame bla/bla", fr );
    box->addWidget( bar );
    box->addWidget( label );
    fr->show();
    label->show();
    bar->show();
    fr->repaint();
    qApp->processEvents();

    // open files

    int infd = ::open( (const char*) infile, O_RDONLY );
    if ( infd == -1 )
    {
        owarn << "couldn't open capture file: " << strerror(errno) << oendl;
        return;
    }

    int outfd = ::open( (const char*) outfile, O_CREAT | O_WRONLY | O_TRUNC, 0644 );
    if ( outfd == -1 )
    {
        owarn << "couldn't open output file: " << strerror(errno) << oendl;
        return;
    }

    int framesize = captureX*captureY*2;

    unsigned char* inbuffer = new unsigned char[ framesize ];
    QImage image;

    avi_start( outfd, _videopics ); // write preambel

    // post process

    for ( int i = 0; i < _videopics; ++i )
    {
        odebug << "processing frame " << i << oendl;

        // <gui>
        label->setText( QString().sprintf( "Post processing frame %d / %d", i+1, _videopics ) );
        bar->setProgress( i );
        bar->repaint();
        qApp->processEvents();
        // </gui>

        int read = ::read( infd, inbuffer, framesize );
        odebug << "read " << read << " bytes" << oendl;
        bufferToImage( captureX, captureY, inbuffer, &image );

        QPixmap p;
        p.convertFromImage( image );
        preview->setPixmap( p );
        preview->repaint();
        qApp->processEvents();

        QString tmpfilename( "/tmp/tempfile" );
        //tmpfilename.sprintf( "/tmp/test/%d.jpg", i );

        imageToFile( &image, tmpfilename, "JPEG", quality );

        QFile framefile( tmpfilename );
        if ( !framefile.open( IO_ReadOnly ) )
        {
            oerr << "can't process file: %s" << strerror(errno) << oendl;
            return; // TODO: clean up temp ressources
        }

        int filesize = framefile.size();
        odebug << "filesize for frame " << i << " = " << filesize << oendl;

        unsigned char* tempbuffer = new unsigned char[ filesize ];
        framefile.readBlock( (char*) tempbuffer, filesize );
        avi_add( outfd, tempbuffer, filesize );
        delete tempbuffer;
        framefile.close();
    }

    avi_end( outfd, captureX, captureY, _framerate );

    fr->hide();
    delete fr;

    updateCaption();

}


void CameraMainWindow::updateCaption()
{
    if ( !_capturing )
        setCaption( QString().sprintf( "Opie-Camera: %dx%d %s q%d z%d (%s)", captureX, captureY, (const char*) captureFormat.lower(), quality, zoom, (const char*) flip ) );
    else
        setCaption( "Opie-Camera: => CAPTURING <=" );
}


#ifndef QT_NO_DEBUG
void CameraMainWindow::doSomething()
{
    captureX = 240;
    captureY = 320;
    _videopics = 176;
    _framerate = 5;
    postProcessVideo( "/var/compile/opie/noncore/multimedia/camera/capture-320x240.dat",
    "/tmp/output.avi" );
}
#endif

