#include <qstring.h>
#include <qlayout.h>
#include <qframe.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qfont.h>

#include <assert.h>

#include "krfbconnection.h"
#include "kvncoptionsdlg.h"
#include "kvncconnectdlg.h"


KVNCConnectDlg::KVNCConnectDlg( KRFBConnection *con,
				QWidget *parent, const char *name )
  : QDialog( parent, name, true )
{
  setCaption( tr("Connect to VNC server") );
  assert( con );
  this->con = con;

  QGridLayout *inner = new QGridLayout( this, 3, 2, 6 );

  QLabel *label = new QLabel( tr("Host Name:"), 
			      this , "hostLabel");
  hostNameCombo = new QComboBox( true, this );
  hostNameCombo->setInsertionPolicy( QComboBox::AtTop );
  hostNameCombo->setMaxCount( 10 );
  hostNameCombo->insertItem( "localhost" );
  hostNameCombo->setFocus();

  inner->addWidget( label, 0, 0 );
  inner->addWidget( hostNameCombo, 0, 1 );

  label = new QLabel( tr("Display Number:"), this, "displayNumber" );
  displayNumberEdit = new QSpinBox( this );

  inner->addWidget( label, 1, 0 );
  inner->addWidget( displayNumberEdit, 1, 1 );

  //  if ( viewer->display() != -1 ) {
  //    displayNumberEdit->setValue( viewer->display() );
    displayNumberEdit->setValue( 1 );
    //  }

  label = new QLabel( tr("Password:"), this );
  inner->addWidget( label, 2, 0 );

  passwordEdit = new QLineEdit( this );
  passwordEdit->setEchoMode( QLineEdit::Password );
  inner->addWidget( passwordEdit, 2, 1 );

  inner->setColStretch( 0, 0 );
  inner->setColStretch( 1, 15 );
}


void KVNCConnectDlg::accept()
{
  int dis;
  //  viewer->setHost(hostNameCombo->currentText());
  QString temp = displayNumberEdit->text();
  if(temp.isEmpty())
    dis = -1;
  else
    dis = temp.toUInt();    
  //  viewer->setDisplay(dis);
  QDialog::accept();
}

void KVNCConnectDlg::options()
{
  KVNCOptionsDlg *wdg = new KVNCOptionsDlg( con->options(), this );
  wdg->exec();
  delete wdg;
}

