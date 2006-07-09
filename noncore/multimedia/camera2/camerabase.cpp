/****************************************************************************
** Form implementation generated from reading ui file 'camerabase.ui'
**
** Created: Mon Jul 10 04:21:25 2006
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "camerabase.h"

#include <qframe.h>
#include <qpushbutton.h>
#include "thumbbutton.h"
#include "videocaptureview.h"
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a CameraBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
CameraBase::CameraBase( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "camera" );
    resize( 311, 381 ); 
    setCaption( tr( "Camera" ) );
    cameraLayout = new QGridLayout( this ); 
    cameraLayout->setSpacing( 0 );
    cameraLayout->setMargin( 0 );

    Frame3 = new QFrame( this, "Frame3" );
    Frame3->setFrameShape( QFrame::Panel );
    Frame3->setFrameShadow( QFrame::Sunken );
    Frame3->setLineWidth( 4 );
    Frame3->setMargin( 4 );
    Frame3->setMidLineWidth( 3 );
    Frame3Layout = new QVBoxLayout( Frame3 ); 
    Frame3Layout->setSpacing( 6 );
    Frame3Layout->setMargin( 4 );

    videocaptureview = new VideoCaptureView( Frame3, "videocaptureview" );
    videocaptureview->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, videocaptureview->sizePolicy().hasHeightForWidth() ) );
    Frame3Layout->addWidget( videocaptureview );

    cameraLayout->addMultiCellWidget( Frame3, 0, 0, 0, 1 );

    photo = new QPushButton( this, "photo" );
    photo->setText( tr( "Photo" ) );

    cameraLayout->addWidget( photo, 1, 0 );

    video = new QPushButton( this, "video" );
    video->setText( tr( "Video" ) );

    cameraLayout->addWidget( video, 1, 1 );

    thumbs = new QFrame( this, "thumbs" );
    thumbs->setFrameShape( QFrame::NoFrame );
    thumbs->setFrameShadow( QFrame::Plain );
    thumbsLayout = new QHBoxLayout( thumbs ); 
    thumbsLayout->setSpacing( 0 );
    thumbsLayout->setMargin( 0 );

    thumb1 = new ThumbButton( thumbs, "thumb1" );
    thumbsLayout->addWidget( thumb1 );

    thumb2 = new ThumbButton( thumbs, "thumb2" );
    thumbsLayout->addWidget( thumb2 );

    thumb3 = new ThumbButton( thumbs, "thumb3" );
    thumbsLayout->addWidget( thumb3 );

    thumb4 = new ThumbButton( thumbs, "thumb4" );
    thumbsLayout->addWidget( thumb4 );

    thumb5 = new ThumbButton( thumbs, "thumb5" );
    thumbsLayout->addWidget( thumb5 );

    cameraLayout->addMultiCellWidget( thumbs, 2, 2, 0, 1 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
CameraBase::~CameraBase()
{
    // no need to delete child widgets, Qt does it all for us
}

