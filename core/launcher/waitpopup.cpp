#include <qlabel.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qmovie.h>

#include "waitpopup.h"
#include <qpe/resource.h>


WaitPopup::WaitPopup(QWidget *parent, const char* msg, bool dispIcon )
  :QDialog(parent, QObject::tr("Wait"), TRUE,WStyle_Customize) {
  QHBoxLayout *hbox  = new QHBoxLayout( this );
  QLabel *lb;
  lb = new QLabel( this );
  //lb->setMovie( QMovie( "./wait.gif"  , 200) );
  lb->setPixmap( Resource::loadPixmap( "Clock" ) );
  lb->setBackgroundMode ( NoBackground );
  hbox->addWidget( lb );
  hbox->activate();
}

WaitPopup::~WaitPopup() {
}
