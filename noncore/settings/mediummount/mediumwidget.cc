

#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qabstractlayout.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qvbox.h>
#include <qwhatsthis.h>


#include <qpe/config.h>
#include <qpe/qpeapplication.h>

#include "mediumwidget.h"



using namespace MediumMountSetting;

MediumMountWidget::MediumMountWidget(const QString &path,
             const QPixmap &pix,
             QWidget *parent,
             const char *name )
  : QWidget( parent, name )
{
  if(parent == 0){
    resize(QApplication::desktop()->width(), QApplication::desktop()->height() );
  }else{
    resize(parent->width(), parent->height() );
  }
  m_path = path;
  initGUI();
  m_label->setPixmap(pix );
  m_config = 0;  

  readConfig();
}
// now we fire up the GUI
// if I would know what I'm doing ;)
void MediumMountWidget::initGUI()
{
  //main layout
  m_box = new QVBoxLayout( this , 5, 5 );
  m_box->setSpacing( 5 );
  //m_box->addStretch( -1 );

  // picture + text
  m_infoBox = new QHBox(this, "infobox" );
  m_infoBox->setSpacing( 4 );
  m_label = new QLabel(m_infoBox );
  m_desc = new QLabel(m_infoBox );
  m_desc->setTextFormat( Qt::RichText );
  QWhatsThis::add( this, tr("Configure this medium. The changes will"
      " go into effect when the application gets"
      " closed. To update the Document Tab you need"
                            " to remove and insert this medium."));
  m_desc->setText("" );
  m_box->addWidget( m_infoBox ); // add the widget to the layout


  // groupbox
  m_group = new QGroupBox(tr("Which media files"), this, "MediaFiles" );
  m_checks = new QGridLayout( m_group, 4, 3 );
  m_checks->setMargin( 12 );



  QSpacerItem *item2 = new QSpacerItem(5, 8,
               QSizePolicy::Fixed,
               QSizePolicy::Fixed);
  m_box->addItem( item2 );

  m_audio = new QCheckBox( tr("Audio"), m_group );
  m_all   = new QCheckBox( tr("All")  , m_group );
  m_image = new QCheckBox( tr("Image"), m_group );
  m_text  = new QCheckBox( tr("Text") , m_group );
  m_video = new QCheckBox( tr("Video"), m_group );

  QSpacerItem *iti1b = new QSpacerItem(2, 10, QSizePolicy::Fixed, 
               QSizePolicy::Fixed );
  m_checks->addItem( iti1b, 0, 0 );

  m_checks->addWidget(m_audio, 1, 0 );
  m_checks->addWidget(m_image, 2, 0 );
  m_checks->addWidget(m_all  , 3, 0 );

  m_checks->addWidget(m_text,  1, 2 );
  m_checks->addWidget(m_video, 2, 2 );

  m_checks->addRowSpacing(0, 8 );
  m_checks->addColSpacing(1, 2 );
  m_checks->setColStretch(1, -2 );

  connect(m_all, SIGNAL(stateChanged(int) ), 
    this, SLOT(slotStateChanged() ) );

  m_box->addWidget( m_group );

  // label
  m_lblPath = new QLabel(tr("Limit search to:"), this );
  m_box->addWidget( m_lblPath );

  // add to
  m_hboxAdd = new QHBox( this );
  m_hboxAdd->setSpacing( 10 );
  m_edit = new QLineEdit(m_hboxAdd );
  m_add = new QPushButton(m_hboxAdd );
  m_add->setText( tr("Add") );

  m_box->addWidget(m_hboxAdd );

  m_always = new QCheckBox( tr("Always check this medium"), this );

  m_box->addWidget( m_always );

  QSpacerItem *item = new QSpacerItem(5, 50,
              QSizePolicy::Fixed,
              QSizePolicy::Expanding );
  m_box->addItem(item );
}

void MediumMountWidget::readConfig( )
{
  if( m_config == 0 )
    m_config = new Config(m_path + "/.opiestorage.cf", Config::File );
  m_config->setGroup( "main" );

  m_always->setChecked( m_config->readBoolEntry("check", false)  );

  m_config->setGroup( "mimetypes" );
  if( m_config->readBoolEntry("all", false ) ){
    m_audio->setEnabled( false );
    m_image->setEnabled( false );
    m_text->setEnabled ( false );
    m_video->setEnabled( false );
    m_all->setChecked( true );
  }else{
    m_audio->setEnabled( true );
    m_image->setEnabled( true );
    m_text->setEnabled ( true );
    m_all->setEnabled  ( true ); 

    m_all->setChecked( false );

    m_audio->setChecked( m_config->readBoolEntry("audio", true ) );
    m_image->setChecked( m_config->readBoolEntry("image", true ) );
    m_text->setChecked ( m_config->readBoolEntry("text" , true ) );
    m_video->setChecked( m_config->readBoolEntry("video", true ) );
  };
}

void MediumMountWidget::writeConfig()
{
  m_config->setGroup("main");
  m_config->writeEntry("check", m_always->isChecked() );

  m_config->setGroup("mimetypes" );
  if(m_all->isChecked() ){
    m_config->writeEntry("all", true );
  }else{
    m_config->writeEntry("audio", m_audio->isChecked() );
    m_config->writeEntry("image", m_image->isChecked() );
    m_config->writeEntry("text" , m_text->isChecked()  );
    m_config->writeEntry("video", m_video->isChecked() );
  }
}
MediumMountWidget::~MediumMountWidget()
{
  writeConfig();
  delete m_config;
}


void MediumMountWidget::slotAdd()
{

}
void MediumMountWidget::slotStateChanged()
{
  bool state = !(m_all->isChecked());

  m_audio->setEnabled( state );
  m_text->setEnabled ( state );
  m_video->setEnabled( state );
  m_image->setEnabled( state );

}
