/****************************************************************************
** Form implementation generated from reading ui file 'camerasettings.ui'
**
** Created: Mon Jul 10 04:21:25 2006
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "camerasettings.h"

#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qtopia/locationcombo.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a CameraSettings which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
CameraSettings::CameraSettings( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "CameraSettings" );
    resize( 324, 465 ); 
    setCaption( tr( "Settings" ) );
    CameraSettingsLayout = new QVBoxLayout( this ); 
    CameraSettingsLayout->setSpacing( 6 );
    CameraSettingsLayout->setMargin( 11 );

    location = new LocationCombo( this, "location" );
    CameraSettingsLayout->addWidget( location );

    photo = new QGroupBox( this, "photo" );
    photo->setTitle( tr( "Photo" ) );
    photo->setColumnLayout(0, Qt::Vertical );
    photo->layout()->setSpacing( 0 );
    photo->layout()->setMargin( 0 );
    photoLayout = new QVBoxLayout( photo->layout() );
    photoLayout->setAlignment( Qt::AlignTop );
    photoLayout->setSpacing( 2 );
    photoLayout->setMargin( 4 );

    Layout1 = new QHBoxLayout; 
    Layout1->setSpacing( 6 );
    Layout1->setMargin( 0 );

    TextLabel1 = new QLabel( photo, "TextLabel1" );
    TextLabel1->setText( tr( "Size" ) );
    Layout1->addWidget( TextLabel1 );

    photo_size = new QComboBox( FALSE, photo, "photo_size" );
    photo_size->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, photo_size->sizePolicy().hasHeightForWidth() ) );
    Layout1->addWidget( photo_size );
    photoLayout->addLayout( Layout1 );

    Layout2 = new QHBoxLayout; 
    Layout2->setSpacing( 6 );
    Layout2->setMargin( 0 );

    TextLabel2 = new QLabel( photo, "TextLabel2" );
    TextLabel2->setText( tr( "Quality" ) );
    Layout2->addWidget( TextLabel2 );

    photo_quality = new QSlider( photo, "photo_quality" );
    photo_quality->setMinValue( 0 );
    photo_quality->setMaxValue( 100 );
    photo_quality->setValue( 75 );
    photo_quality->setOrientation( QSlider::Horizontal );
    Layout2->addWidget( photo_quality );

    photo_quality_n = new QLabel( photo, "photo_quality_n" );
    photo_quality_n->setText( tr( "75" ) );
    Layout2->addWidget( photo_quality_n );
    photoLayout->addLayout( Layout2 );
    CameraSettingsLayout->addWidget( photo );

    video = new QGroupBox( this, "video" );
    video->setTitle( tr( "Video" ) );
    video->setColumnLayout(0, Qt::Vertical );
    video->layout()->setSpacing( 0 );
    video->layout()->setMargin( 0 );
    videoLayout = new QVBoxLayout( video->layout() );
    videoLayout->setAlignment( Qt::AlignTop );
    videoLayout->setSpacing( 2 );
    videoLayout->setMargin( 4 );

    Layout1_2 = new QHBoxLayout; 
    Layout1_2->setSpacing( 6 );
    Layout1_2->setMargin( 0 );

    TextLabel1_2 = new QLabel( video, "TextLabel1_2" );
    TextLabel1_2->setText( tr( "Size" ) );
    Layout1_2->addWidget( TextLabel1_2 );

    video_size = new QComboBox( FALSE, video, "video_size" );
    video_size->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, video_size->sizePolicy().hasHeightForWidth() ) );
    Layout1_2->addWidget( video_size );
    videoLayout->addLayout( Layout1_2 );

    Layout2_2 = new QHBoxLayout; 
    Layout2_2->setSpacing( 6 );
    Layout2_2->setMargin( 0 );

    TextLabel2_2 = new QLabel( video, "TextLabel2_2" );
    TextLabel2_2->setText( tr( "Quality" ) );
    Layout2_2->addWidget( TextLabel2_2 );

    video_quality = new QSlider( video, "video_quality" );
    video_quality->setMinValue( 0 );
    video_quality->setMaxValue( 100 );
    video_quality->setValue( 75 );
    video_quality->setOrientation( QSlider::Horizontal );
    Layout2_2->addWidget( video_quality );

    video_quality_n = new QLabel( video, "video_quality_n" );
    video_quality_n->setText( tr( "75" ) );
    Layout2_2->addWidget( video_quality_n );
    videoLayout->addLayout( Layout2_2 );

    Layout6 = new QHBoxLayout; 
    Layout6->setSpacing( 6 );
    Layout6->setMargin( 0 );

    TextLabel4 = new QLabel( video, "TextLabel4" );
    TextLabel4->setText( tr( "Frame rate" ) );
    Layout6->addWidget( TextLabel4 );

    video_framerate = new QSlider( video, "video_framerate" );
    video_framerate->setMinValue( 1 );
    video_framerate->setMaxValue( 60 );
    video_framerate->setValue( 5 );
    video_framerate->setOrientation( QSlider::Horizontal );
    Layout6->addWidget( video_framerate );

    TextLabel6 = new QLabel( video, "TextLabel6" );
    TextLabel6->setText( tr( "5" ) );
    Layout6->addWidget( TextLabel6 );
    videoLayout->addLayout( Layout6 );
    CameraSettingsLayout->addWidget( video );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    CameraSettingsLayout->addItem( spacer );

    // signals and slots connections
    connect( photo_quality, SIGNAL( valueChanged(int) ), photo_quality_n, SLOT( setNum(int) ) );
    connect( video_quality, SIGNAL( valueChanged(int) ), video_quality_n, SLOT( setNum(int) ) );
    connect( video_framerate, SIGNAL( valueChanged(int) ), TextLabel6, SLOT( setNum(int) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
CameraSettings::~CameraSettings()
{
    // no need to delete child widgets, Qt does it all for us
}

