

#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qabstractlayout.h> // spacer item
#include <qlayout.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qwhatsthis.h>

#include <qpe/config.h>

#include "mediumglobal.h"

using namespace MediumMountSetting;

MediumGlobalWidget::MediumGlobalWidget(QWidget *wid, const char *name )
  : QWidget( wid, name )
{
  m_config = 0;
  initGUI();
  readConfig();

}
void MediumGlobalWidget::initGUI()
{
  m_layout = new QVBoxLayout(this );
  m_layout->setMargin( 10 );
  m_layout->setSpacing( 10 );


  m_label = new QLabel( this );
  m_label->setTextFormat( Qt::RichText );
  m_label->setText( tr("") );
  QWhatsThis::add( this, ("If a medium gets inserted into this device Opie "
           "tries to search the medium for Dcouments. On "
           "large mediums this can take some time. You can choose "
           "if Opie should scan for Documents globally or on a "
           "per medium level. You're also able to reconfigure "
           "each medium."
           ) );
  
  m_layout->addWidget( m_label );

  m_check = new QCheckBox( tr("Enable medium checking" ), this );
  connect( m_check, SIGNAL(stateChanged(int) ),
     this, SLOT(slotEnableChecking() ) );
  m_layout->addWidget(m_check );

  m_frame = new QFrame(this, "Frame" );
  m_frame->setFrameShape( QFrame::Box );
  m_frame->setFrameShadow( QFrame::Sunken );

  m_box = new QVBoxLayout( m_frame );
  m_box->setMargin( 5 );
  m_useglobal = new QCheckBox( tr("Use global settings"), m_frame );
  connect( m_useglobal, SIGNAL( stateChanged(int) ),
     this, SLOT( slotGlobalChanged() ) );

  m_box->addWidget( m_useglobal );

  m_global = new QGroupBox( tr("Which media files"), m_frame );
  m_frameLay = new QGridLayout(m_global, 4, 3 );
  m_frameLay->setMargin( 12 );

  QSpacerItem *item2 = new QSpacerItem( 5, 8,
          QSizePolicy::Fixed,
          QSizePolicy::Fixed );         
  m_audio = new QCheckBox( tr("Audio"), m_global );
  m_all   = new QCheckBox( tr("All")  , m_global );
  m_image = new QCheckBox( tr("Image"), m_global );
  m_text  = new QCheckBox( tr("Text") , m_global );
  m_video = new QCheckBox( tr("Video"), m_global );

  connect(m_all, SIGNAL(stateChanged(int) ),
    this, SLOT(slotAllChanged()  ) );

  m_frameLay->addItem( item2, 0, 0 );

  m_frameLay->addWidget( m_audio, 1, 0 );
  m_frameLay->addWidget( m_image, 2, 0 );
  m_frameLay->addWidget( m_all,   3, 0 );

  m_frameLay->addWidget( m_text,  1, 2 );
  m_frameLay->addWidget( m_video, 2, 2 );

  m_frameLay->addRowSpacing( 0, 8 );
  m_frameLay->addColSpacing( 1, 2 );

  m_box->addWidget( m_global );
  

  m_layout->addWidget( m_frame );

  QSpacerItem *item1 = new QSpacerItem( 1, 24,
          QSizePolicy::Fixed,
          QSizePolicy::Expanding );
  m_layout->addItem( item1 );
}
void MediumGlobalWidget::readConfig()
{
  if( m_config == 0 )
    m_config = new Config("medium" );

  m_config->setGroup("main");
  m_useglobal->setChecked( m_config->readBoolEntry("global", false ) );
  m_check->setChecked( m_config->readBoolEntry("use", true ) );

  m_config->setGroup("mimetypes" );
  m_all->setChecked  ( m_config->readBoolEntry("all", false ) );
  m_audio->setChecked( m_config->readBoolEntry("audio", true ) );
  m_video->setChecked( m_config->readBoolEntry("video", true ) );
  m_text->setChecked ( m_config->readBoolEntry("text", true ) );
  m_image->setChecked( m_config->readBoolEntry("image", true ) );

  slotAllChanged();
  slotEnableChecking();
  slotGlobalChanged();
  if( m_all->isChecked() ){
    m_video->setEnabled( false );
    m_text->setEnabled(  false );
    m_audio->setEnabled( false );
    m_image->setEnabled( false );

  }
}
void MediumGlobalWidget::writeConfig()
{
  m_config->setGroup( "main" );
  m_config->writeEntry("global", m_useglobal->isChecked() );
  m_config->writeEntry("use", m_check->isChecked() );

  m_config->setGroup("mimetypes" );

  m_config->writeEntry("all", m_all->isChecked() );
  m_config->writeEntry("audio", m_audio->isChecked() );
  m_config->writeEntry("video", m_video->isChecked() );
  m_config->writeEntry("text", m_text->isChecked() );
  m_config->writeEntry("image", m_image->isChecked() );
}
MediumGlobalWidget::~MediumGlobalWidget()
{
  writeConfig();
  delete m_config;
}
void MediumGlobalWidget::slotGlobalChanged()
{
  int mode = GLOBAL_DISABLED;
  bool enabled = false;
  if( ( enabled =m_useglobal->isChecked() ) ){
    mode = GLOBAL_ENABLED;
  }else
    mode = GLOBAL_DISABLED;
  qWarning("enabled = %d", enabled );
  m_all->setEnabled  ( enabled );
  m_audio->setEnabled( enabled );
  m_image->setEnabled( enabled );
  m_text->setEnabled ( enabled );
  m_video->setEnabled ( enabled );
  slotAllChanged();

  emit globalStateChanged( mode );
}
void MediumGlobalWidget::slotEnableChecking()
{
  int mode = ENABLE_CHECKS;
  bool enabled = false;
  if( ( enabled = m_check->isChecked() ) ){
    mode = ENABLE_CHECKS;
  }else{
    mode = DISABLE_CHECKS;
  }
  m_frame->setEnabled( enabled );
  slotGlobalChanged();
  emit enableStateChanged( mode );
}
void MediumGlobalWidget::slotAllChanged()
{
  bool enable = !m_all->isChecked();
  m_audio->setEnabled( enable );
  m_text->setEnabled( enable );
  m_video->setEnabled( enable );
  m_image->setEnabled( enable );
}
