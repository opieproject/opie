
#include "mediummountgui.h"

#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qvbox.h>
#include <qhbox.h>

#include <qpe/resource.h>
#include <qpe/config.h>


MediumMountGui::MediumMountGui( Config *cfg, const QString &path ,QWidget* parent,  const char* name, bool , WFlags  )
  : QDialog( parent, name, true ) {

    m_cfg = cfg;
    setCaption( tr( "Medium inserted" ) );

    mediumPath = path;
    readConfig();
    startGui();
    showMaximized();
}

void MediumMountGui::readConfig() {

    //OConfig cfg (mediumPath +"/.opiestorage.cf");
    m_cfg->setGroup( "main" );
    checkagain = m_cfg->readBoolEntry( "check", false );

    m_cfg->setGroup( "mimetypes" );
    checkmimeaudio = m_cfg->readBoolEntry( "audio", true );
    checkmimeimage = m_cfg->readBoolEntry( "image", true );
    checkmimetext  = m_cfg->readBoolEntry( "text" , true );
    checkmimevideo = m_cfg->readBoolEntry( "video", true );
    checkmimeall   = m_cfg->readBoolEntry( "all"  , false );

    m_cfg->setGroup( "dirs" );
    limittodirs = m_cfg->readEntry( "dirs", "" );
}

bool MediumMountGui::check() {
    return !checkagain;
}

QStringList MediumMountGui::dirs() {
    return QStringList::split( ",", limittodirs );
}

void MediumMountGui::writeConfig( bool autocheck ) {

    //OConfig cfg (mediumPath +"/.opiestorage.cf");
    m_cfg->setGroup( "main" );
    m_cfg->writeEntry( "check", AskBox->isChecked() );
    m_cfg->writeEntry( "autocheck", autocheck );

    m_cfg->setGroup( "mimetypes" );

    m_cfg->writeEntry( "audio", CheckBoxAudio->isChecked() );
    m_cfg->writeEntry( "image", CheckBoxImage->isChecked() );
    m_cfg->writeEntry( "text", CheckBoxImage->isChecked() );
    m_cfg->writeEntry( "video", CheckBoxVideo->isChecked() );
    m_cfg->writeEntry( "all", CheckBoxAll->isChecked() );

    m_cfg->setGroup( "dirs" );
    m_cfg->writeEntry( "dirs", "" );

    // if all is checked then add only "QString::null" to the list.
    if ( checkmimeall ) {
        mimeTypeList.clear();
        mimeTypeList += QString::null;
    } else {
        if ( checkmimeaudio ) {
            mimeTypeList += ( "audio/*" );
        }
        if ( checkmimetext ) {
            mimeTypeList += ( "text/*" );
        }
        if ( checkmimevideo ) {
            mimeTypeList += ( "video/*" );
        }
        if ( checkmimeimage ) {
            mimeTypeList += ( "image/*" );
        }
    }
    m_cfg->write(); // not really needed here but just to be sure
}

void MediumMountGui::startGui() {

    QVBoxLayout* mainLayout = new QVBoxLayout( this );

    QHBoxLayout * titelBox = new QHBoxLayout( mainLayout );
    Text_2 = new QLabel( this );
    // FIXME needs an image
    Text_2->setPixmap( Resource::loadPixmap( "mediummount/mediummount" ) );

    Text = new QLabel( this, "Text" );
    Text->setText( tr( "A <b>storage medium</b> was inserted. Should it be scanned for media files?" ) );
    titelBox->addWidget( Text_2 );
    titelBox->setStretchFactor( Text_2, 2 );
    titelBox->addWidget( Text );
    titelBox->setStretchFactor( Text, 8 );

    // media box
    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setTitle( tr( "Which media files" ) );

    QGridLayout *mimeLayout = new QGridLayout( GroupBox1, 3, 2, 10 );

    CheckBoxAudio = new QCheckBox( GroupBox1, "CheckBoxAudio" );
    CheckBoxAudio->setText( tr( "Audio" ) );

    CheckBoxImage = new QCheckBox( GroupBox1, "CheckBoxImage" );
    CheckBoxImage->setText( tr( "Image" ) );

    CheckBoxText = new QCheckBox( GroupBox1, "CheckBoxText" );
    CheckBoxText->setText( tr( "Text" ) );

    CheckBoxVideo = new QCheckBox( GroupBox1, "CheckBoxVideo" );
    CheckBoxVideo->setText( tr( "Video" ) );

    CheckBoxAll = new QCheckBox ( GroupBox1 );
    CheckBoxAll->setText( tr( "All" ) );

    CheckBoxLink = new QCheckBox ( GroupBox1 );
    CheckBoxLink->setText( tr( "Link apps" ) );
    // as long as the feature is not supported
    CheckBoxLink->setEnabled( false );

    mimeLayout->addWidget( CheckBoxAudio, 0, 0 );
    mimeLayout->addWidget( CheckBoxImage, 0, 1 );
    mimeLayout->addWidget( CheckBoxText , 1, 0 );
    mimeLayout->addWidget( CheckBoxVideo, 1, 1 );
    mimeLayout->addWidget( CheckBoxAll, 2, 0 );
    mimeLayout->addWidget( CheckBoxLink, 2, 1 );

    connect( CheckBoxAll, SIGNAL( clicked() ), this, SLOT( deactivateOthers() ) );

    QVBox* dirBox = new QVBox( this );
    dirBox->setMargin( 4 );

    // select dirs
    DirSelectText = new QLabel( dirBox, "DirSelectText" );
    DirSelectText->setText( tr( "Limit search to dir: (not used yet)" ) );

    QHBox* dirLineBox = new QHBox( dirBox );
    LineEdit1 = new QLineEdit( dirLineBox );
    PushButton3 = new QPushButton( dirLineBox );
    PushButton3->setText( tr( "Add" ) );

    // decision
    DirSelectText_2 = new QLabel( dirBox );
    DirSelectText_2->setText( tr( "Your decision will be stored on the medium." ) );

    // ask again
    AskBox = new QCheckBox( dirBox );
    AskBox->setText( tr( "Do not ask again for this medium" ) );

    AskBox->setChecked( checkagain );
    CheckBoxAudio->setChecked( checkmimeaudio );
    CheckBoxImage->setChecked( checkmimeimage );
    CheckBoxText->setChecked( checkmimetext );
    CheckBoxVideo->setChecked( checkmimevideo );
    CheckBoxAll->setChecked( checkmimeall );
    deactivateOthers();

    QHBox* buttonBox = new QHBox( this );
    // buttons
    quit = new QPushButton( buttonBox );
    quit->setFocus();
    quit->setText( tr( "Yes" ) );
    quit_2 = new QPushButton( buttonBox );
    quit_2->setText( tr( "No" ) );

    mainLayout->addWidget( GroupBox1 );
    mainLayout->addWidget( dirBox );
    mainLayout->addWidget( buttonBox );

    connect( quit, SIGNAL( clicked() ), this, SLOT( yesPressed() ) );
    connect( quit_2, SIGNAL( clicked() ), this, SLOT( noPressed() ) );
}


void MediumMountGui::deactivateOthers() {
    bool mod = !( CheckBoxAll->isChecked() );
    CheckBoxVideo->setEnabled( mod );
    CheckBoxAudio->setEnabled( mod );
    CheckBoxText->setEnabled( mod );
    CheckBoxImage->setEnabled( mod );
}

void MediumMountGui::yesPressed() {
    writeConfig( true );
    // and do something
    accept();
}


QStringList MediumMountGui::mimeTypes() {
    return mimeTypeList;
}

void MediumMountGui::noPressed() {
    writeConfig( false );
    reject();
}

MediumMountGui::~MediumMountGui() {
}

