
#include "mediummountgui.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include <qpe/resource.h>
#include <opie/oconfig.h>


#include <qapplication.h>


MediumMountGui::MediumMountGui( const QString &path ,QWidget* parent,  const char* name, bool modal, WFlags fl )
  : QDialog( parent, name, true ) {
 

  QWidget *d = QApplication::desktop();
  int w=d->width();
  int h=d->height();
  resize( w , h );
  setCaption( tr( "Medium inserted" ) );

  mediumPath = path;
  readConfig();
  startGui();
}

void MediumMountGui::readConfig(){

  OConfig cfg (mediumPath +"/.opiestorage.cf");
  cfg.setGroup("main");
  checkagain = cfg.readBoolEntry("check", false);
  
  cfg.setGroup("mimetypes");
  checkmimeaudio = cfg.readBoolEntry("audio", true);
  checkmimeimage = cfg.readBoolEntry("image", true);
  checkmimetext  = cfg.readBoolEntry("text", true);
  checkmimevideo = cfg.readBoolEntry("video", true);
  checkmimeall   = cfg.readBoolEntry("all", true);

  cfg.setGroup("dirs");
  limittodirs = cfg.readEntry("dirs", "");
}

bool MediumMountGui::check() {
  return !checkagain;
}

QStringList MediumMountGui::dirs() {
  QStringList list = QStringList::split(",", limittodirs );
  return list;
}

void MediumMountGui::writeConfig(bool autocheck) {

  OConfig cfg (mediumPath +"/.opiestorage.cf");
  cfg.setGroup("main");
  cfg.writeEntry("check",  AskBox->isChecked() );
  cfg.writeEntry("autocheck", autocheck );  

  cfg.setGroup("mimetypes");

  cfg.writeEntry("audio", CheckBoxAudio->isChecked() );
  cfg.writeEntry("image",CheckBoxImage->isChecked() );
  cfg.writeEntry("text",CheckBoxImage->isChecked() );
  cfg.writeEntry("video",CheckBoxVideo->isChecked() );
  cfg.writeEntry("all",CheckBoxAll->isChecked() );

  cfg.setGroup("dirs");
  cfg.writeEntry("dirs", ""); 


  // if all is checked then add only "QString::null" to the list.
  if (checkmimeall) {
    mimeTypeList.clear();
    mimeTypeList += QString::null;
  } else {
    if (checkmimeaudio) {
      mimeTypeList += ("audio//*");
    }
    if (checkmimetext) {
      mimeTypeList += ("text//*");
    }
    if (checkmimevideo) {
      mimeTypeList += ("video//*");
    }
    if (checkmimeimage) {
      mimeTypeList += ("image//*");
    }
    if (checkmimeall) {
      mimeTypeList.clear();
      mimeTypeList << QString::null;
    }
  }
  cfg.write(); // not really needed here but just to be sure
}

void MediumMountGui::startGui() {

  QPixmap image = Resource::loadPixmap( "HelpBrowser");

  Text_2 = new QLabel( this );
  Text_2->setGeometry( QRect( 10, 15, 40, 40 ) ); 
  Text_2->setPixmap( image );
  
  Text = new QLabel( this, "Text" );
  Text->setGeometry( QRect( 55, 11, this->width()-50, 50 ) ); 
  Text->setText( tr( "A <b>storage medium</b> was inserted. Should it be scanned for media files?" ) );
  
  // media box
  GroupBox1 = new QGroupBox( this, "GroupBox1" );
  GroupBox1->setGeometry( QRect( 10, 80, (this->width())-25, 80 ) ); 
  GroupBox1->setTitle( tr( "Which media files" ) );
  
  CheckBoxAudio = new QCheckBox( GroupBox1, "CheckBoxAudio" );
  CheckBoxAudio->setGeometry( QRect( 10, 15, (GroupBox1->width()/2)-15 , 15 ) ); 
  CheckBoxAudio->setText( tr( "Audio" ) );
 
  CheckBoxImage = new QCheckBox( GroupBox1, "CheckBoxImage" );
  CheckBoxImage->setGeometry( QRect( 10, 35,(GroupBox1->width()/2)-15, 15 ) ); 
  CheckBoxImage->setText( tr( "Image" ) );

  CheckBoxText = new QCheckBox( GroupBox1, "CheckBoxText" );
  CheckBoxText->setGeometry( QRect((GroupBox1->width()/2) , 15, (GroupBox1->width()/2)-15, 15 ) ); 
  CheckBoxText->setText( tr( "Text" ) );

  CheckBoxVideo = new QCheckBox( GroupBox1, "CheckBoxVideo" );
  CheckBoxVideo->setGeometry( QRect( (GroupBox1->width()/2), 35, (GroupBox1->width()/2)-15, 15 ) ); 
  CheckBoxVideo->setText( tr( "Video" ) );

  CheckBoxAll = new QCheckBox ( GroupBox1);
  CheckBoxAll->setGeometry( QRect( 10, 55, (GroupBox1->width()/2)-15, 15 ) ); 
  CheckBoxAll->setText( tr( "All" ) );
  QObject::connect( (QObject*)CheckBoxAll, SIGNAL( clicked() ), this, SLOT( deactivateOthers()) );
   
  
   
  CheckBoxLink = new QCheckBox ( GroupBox1);
  CheckBoxLink->setGeometry( QRect( (GroupBox1->width()/2), 55, (GroupBox1->width()/2)-15, 15 ) ); 
  CheckBoxLink->setText( tr( "Link apps" ) );
  // as long as the feature is not supported
  CheckBoxLink->setEnabled(false);

  
  // select dirs

  DirSelectText = new QLabel( this, "DirSelectText" );
  DirSelectText->setGeometry( QRect( 10, 160,this->width() , 20 ) ); 
  DirSelectText->setText( tr( "Limit search to dir: (not used yet)" ) );
  
  LineEdit1 = new QLineEdit( this );
  LineEdit1->setGeometry( QRect( 10, 180, (this->width())-60, 20 ) ); 
  
  PushButton3 = new QPushButton( this );
  PushButton3->setGeometry( QRect( (this->width())-40, 180, 30, 20 ) ); 
  PushButton3->setText( tr( "Add" ) );
  

  // decision
  DirSelectText_2 = new QLabel( this );
  DirSelectText_2->setGeometry( QRect( 10,240,this->width(), 15 ) ); 
  DirSelectText_2->setText( tr( "Your decision will be stored on the medium." ) );
  
  // ask again
  AskBox = new QCheckBox( this );
  AskBox->setGeometry( QRect( 10, 215, (this->width())-15 , 15 ) ); 
  AskBox->setText( tr( "Do not ask again for this medium" ) );
  

  AskBox->setChecked(checkagain);
  CheckBoxAudio->setChecked(checkmimeaudio);
  CheckBoxImage->setChecked(checkmimeimage);
  CheckBoxText->setChecked(checkmimetext);
  CheckBoxVideo->setChecked(checkmimevideo);

  // buttons
  quit = new QPushButton( this );
  quit->setGeometry( QRect( (this->width()/2)- 90 , 260, 80, 22 ) ); 
  quit->setFocus();
  quit->setText( tr( "Yes" ) );
  
  quit_2 = new QPushButton( this );
  quit_2->setGeometry( QRect((this->width()/2) , 260, 80, 22 ) ); 
  quit_2->setText( tr( "No" ) );

  QObject::connect( (QObject*)quit, SIGNAL( clicked() ), this, SLOT(yesPressed() ) );
  QObject::connect( (QObject*)quit_2, SIGNAL( clicked() ), this, SLOT(noPressed() ) );


}


void MediumMountGui::deactivateOthers() {
  bool mod = !(CheckBoxAll->isChecked());

  //if (!CheckBoxVideo->isChecked()){
  //  mod = false;
  //} else {
  //  mod = true;
  //}
  CheckBoxVideo->setEnabled(mod);
  CheckBoxAudio->setEnabled(mod);
  CheckBoxText->setEnabled(mod);
  CheckBoxImage->setEnabled(mod);
}

void MediumMountGui::yesPressed() {
  writeConfig(true);
  // and do something
  accept();
}


QStringList MediumMountGui::mimeTypes(){
  return mimeTypeList;
}

void MediumMountGui::noPressed() {
  writeConfig(false);
  reject();
}

MediumMountGui::~MediumMountGui(){
}

