/****************************************************************************
** Form implementation generated from reading ui file 'soundsettingsbase.ui'
**
** Created: Sun May 19 12:11:35 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "soundsettingsbase.h"

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qmainwindow.h>

#include <qpe/qpeapplication.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
"16 16 3 1",
". c None",
"a c #000000",
"# c #ff0000",
"............#...",
"............##..",
"..........#..#..",
".....a..#..#.#..",
"....aa..##.#..#.",
"...aaa...#.##.#.",
"aaaaaa.#.#..#.##",
"aaaaaa.#.##.#.##",
"aaaaaa.#.##.#.##",
"aaaaaa.#.##.#.##",
"aaaaaa.#.#..#.##",
"...aaa...#.##.#.",
"....aa..#..#..#.",
".....a.....#.##.",
"..........#..#..",
"............##.."};

static const char* const image1_data[] = { 
"16 16 3 1",
". c None",
"# c #000000",
"a c #ff0000",
"................",
"................",
"................",
".....#..........",
"....##..........",
"...###...a...a..",
"######..aaa.aaa.",
"######...aaaaa..",
"######....aaa...",
"######...aaaaa..",
"######..aaa.aaa.",
"...###...a...a..",
"....##..........",
".....#..........",
"................",
"................"};


/* 
 *  Constructs a SoundSettingsBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SoundSettingsBase::SoundSettingsBase( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    QPixmap image0( ( const char** ) image0_data );
    QPixmap image1( ( const char** ) image1_data );
    if ( !name )
  setName( "SoundSettingsBase" );
    resize( 255, 301 ); 
    setCaption( tr( "Vmemo Settings" ) );

    SoundSettingsBaseLayout = new QGridLayout( this ); 
    SoundSettingsBaseLayout->setSpacing( 6 );
    SoundSettingsBaseLayout->setMargin( 11 );

    GroupBox3 = new QGroupBox( this, "GroupBox3" );
    GroupBox3->setFrameShape( QGroupBox::Box );
    GroupBox3->setFrameShadow( QGroupBox::Sunken );
    GroupBox3->setTitle( tr( "Levels" ) );
    GroupBox3->setColumnLayout(0, Qt::Vertical );
    GroupBox3->layout()->setSpacing( 0 );
    GroupBox3->layout()->setMargin( 0 );
    GroupBox3Layout = new QVBoxLayout( GroupBox3->layout() );
    GroupBox3Layout->setAlignment( Qt::AlignTop );
    GroupBox3Layout->setSpacing( 2 );
    GroupBox3Layout->setMargin( 2 );

    Layout10 = new QVBoxLayout; 
    Layout10->setSpacing( 2 );
    Layout10->setMargin( 2 );

    Layout16 = new QHBoxLayout; 
    Layout16->setSpacing( 2 );
    Layout16->setMargin( 2 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout16->addItem( spacer );

    PixmapLabel1_2 = new QLabel( GroupBox3, "PixmapLabel1_2" );
    PixmapLabel1_2->setPixmap( image0 );
    PixmapLabel1_2->setScaledContents( FALSE );
    Layout16->addWidget( PixmapLabel1_2 );

    TextLabel1_2 = new QLabel( GroupBox3, "TextLabel1_2" );
    TextLabel1_2->setText( tr( "Loud" ) );
    Layout16->addWidget( TextLabel1_2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout16->addItem( spacer_2 );
    Layout10->addLayout( Layout16 );

    Layout13 = new QHBoxLayout; 
    Layout13->setSpacing( 2 );
    Layout13->setMargin( 2 );

    volLabel = new QLabel( GroupBox3, "volLabel" );
    volLabel->setText( tr( "Output" ) );
    Layout13->addWidget( volLabel );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout13->addItem( spacer_3 );

    micLabel = new QLabel( GroupBox3, "micLabel" );
    micLabel->setText( tr( "Mic" ) );
    Layout13->addWidget( micLabel );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout13->addItem( spacer_4 );
    Layout10->addLayout( Layout13 );

    Layout12 = new QHBoxLayout; 
    Layout12->setSpacing( 2 );
    Layout12->setMargin( 2 );

    volume = new QSlider( GroupBox3, "volume" );
    volume->setMaxValue( 100 );
    volume->setValue( 50 );
    volume->setOrientation( QSlider::Vertical );
    volume->setTickmarks( QSlider::Right );
    volume->setTickInterval( 5 );
    Layout12->addWidget( volume );
    QSpacerItem* spacer_5 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout12->addItem( spacer_5 );

    mic = new QSlider( GroupBox3, "mic" );
    mic->setMaxValue( 100 );
    mic->setValue( 50 );
    mic->setOrientation( QSlider::Vertical );
    mic->setTickmarks( QSlider::Right );
    mic->setTickInterval( 5 );
    Layout12->addWidget( mic );
    QSpacerItem* spacer_6 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout12->addItem( spacer_6 );
    Layout10->addLayout( Layout12 );

    Layout17 = new QHBoxLayout; 
    Layout17->setSpacing( 2 );
    Layout17->setMargin( 2 );
    QSpacerItem* spacer_7 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout17->addItem( spacer_7 );

    PixmapLabel2_2 = new QLabel( GroupBox3, "PixmapLabel2_2" );
    PixmapLabel2_2->setPixmap( image1 );
    PixmapLabel2_2->setScaledContents( FALSE );
    Layout17->addWidget( PixmapLabel2_2 );

    TextLabel2_2 = new QLabel( GroupBox3, "TextLabel2_2" );
    TextLabel2_2->setText( tr( "Silent" ) );
    Layout17->addWidget( TextLabel2_2 );
    QSpacerItem* spacer_8 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout17->addItem( spacer_8 );
    Layout10->addLayout( Layout17 );
    GroupBox3Layout->addLayout( Layout10 );

    SoundSettingsBaseLayout->addWidget( GroupBox3, 0, 0 );

    Layout12_2 = new QVBoxLayout; 
    Layout12_2->setSpacing( 2 );
    Layout12_2->setMargin( 2 );

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setTitle( tr( "Vmemo" ) );

    QWidget* privateLayoutWidget = new QWidget( GroupBox1, "Layout11" );
    privateLayoutWidget->setGeometry( QRect( 5, 16, 96, 230 ) ); 

    Layout11 = new QVBoxLayout( privateLayoutWidget ); 
    Layout11->setSpacing( 2 );
    Layout11->setMargin( 0 );

    sampleRateLabel = new QLabel( privateLayoutWidget, "sampleRateLabel" );
    sampleRateLabel->setText( tr( "Sample Rate" ) );
    Layout11->addWidget( sampleRateLabel );

    sampleRate = new QComboBox( FALSE, privateLayoutWidget, "sampleRate" );
    sampleRate->insertItem( tr( "8000" ) );
    sampleRate->insertItem( tr( "11025" ) );
    sampleRate->insertItem( tr( "22050" ) );
    sampleRate->insertItem( tr( "33075" ) );
    sampleRate->insertItem( tr( "44100" ) );
    sampleRate->setFixedWidth(90);
    Layout11->addWidget( sampleRate );

    stereoCheckBox = new QCheckBox( privateLayoutWidget, "stereoCheckBox" );
    stereoCheckBox->setText( tr( "Stereo" ) );
    Layout11->addWidget( stereoCheckBox );

    sixteenBitCheckBox = new QCheckBox( privateLayoutWidget, "sixteenBitCheckBox" );
    sixteenBitCheckBox->setText( tr( "16 bit" ) );
    Layout11->addWidget( sixteenBitCheckBox );

    AlertCheckBox = new QCheckBox( privateLayoutWidget, "AlertCheckBox" );
    AlertCheckBox->setText( tr( "Visual Alerts" ) );
    Layout11->addWidget( AlertCheckBox );

    TextLabel1 = new QLabel( privateLayoutWidget, "TextLabel1" );
    TextLabel1->setText( tr( "Location:" ) );
    Layout11->addWidget( TextLabel1 );


    LocationComboBox = new QComboBox( FALSE, privateLayoutWidget, "LocationComboBox" );
    Layout11->addWidget( LocationComboBox );

    QLabel *TextLabelKey;
    TextLabelKey = new QLabel( privateLayoutWidget, "TextLabelKey" );
    TextLabelKey->setText( tr( "Record Key:" ) );
    Layout11->addWidget( TextLabelKey );

    keyComboBox = new QComboBox( FALSE, privateLayoutWidget, "keyComboBox" );
    keyComboBox->insertItem( tr( "" ) );
    keyComboBox->insertItem( tr( "Taskbar Icon" ) );
    keyComboBox->insertItem( tr( "Key_Escape" ) );
    keyComboBox->insertItem( tr( "Key_Space" ) );
    keyComboBox->insertItem( tr( "Key_Home" ) );
    keyComboBox->insertItem( tr( "Key_Calender" ) );
    keyComboBox->insertItem( tr( "Key_Contacts" ) );
    keyComboBox->insertItem( tr( "Key_Menu" ) );
    keyComboBox->insertItem( tr( "Key_Mail" ) );

    Layout11->addWidget( keyComboBox );

    QLabel *timeLimitLabel;
    timeLimitLabel= new QLabel( privateLayoutWidget, "timeLimitLabel" );
    timeLimitLabel->setText( tr( "Recording Limit:" ) );
    Layout11->addWidget( timeLimitLabel );

    timeLimitComboBox = new QComboBox( FALSE, privateLayoutWidget, "timeLimitComboBox" );
    timeLimitComboBox->insertItem( tr( "30" ) );
    timeLimitComboBox->insertItem( tr( "20" ) );
    timeLimitComboBox->insertItem( tr( "15" ) );
    timeLimitComboBox->insertItem( tr( "10" ) );
    timeLimitComboBox->insertItem( tr( "5" ) );

    Layout11->addWidget(timeLimitComboBox);
    QLabel *timeLimitLabel2;
    timeLimitLabel2= new QLabel( privateLayoutWidget, "timeLimitLabel2" );
    timeLimitLabel2->setText( tr( "seconds" ) );
    Layout11->addWidget( timeLimitLabel2 );
   
    Layout12_2->addWidget( GroupBox1 );
// //     touchsound = new QCheckBox( this, "touchsound" );
// //     touchsound->setText( tr( "Screen sounds" ) );
// //     Layout12_2->addWidget( touchsound );

// //     keysound = new QCheckBox( this, "keysound" );
// //     keysound->setText( tr( "Keyboard sounds" ) );
// //     Layout12_2->addWidget( keysound );

    SoundSettingsBaseLayout->addLayout( Layout12_2, 0, 1 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
SoundSettingsBase::~SoundSettingsBase()
{
    // no need to delete child widgets, Qt does it all for us
}
