
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>

#include "auth.h"
#include "interfaceppp.h"
#include "modem.h"
#include "pppdata.h"

InterfacePPP::InterfacePPP(QObject *parent, const char *name, bool status)
    : Interface(parent, name, status),
      _modemPtr(0),
      _dataPtr(0)
{
    odebug << "InterfacePPP::InterfacePPP(" << oendl; 
}

PPPData* InterfacePPP::data()const
{
    if (!_dataPtr){
        odebug << "creating new Data obj" << oendl; 
        _dataPtr = new PPPData();
        _dataPtr->setDevice( getInterfaceName() );
        _dataPtr->setAccount( getHardwareName() );
    }
    return _dataPtr;
}

Modem* InterfacePPP::modem()const
{
    if (!_modemPtr){
        odebug << "creating new modem obj" << oendl; 
        _modemPtr = new Modem( data() );
    }
    return _modemPtr;
}

bool InterfacePPP::refresh()
{
    odebug << "InterfacePPP::refresh()" << oendl; 
    QString old = getInterfaceName();
    setInterfaceName( modem()->pppDevice() );

    (void)Interface::refresh();

    setInterfaceName( old );
    emit updateInterface(this);

    return true;
}

void InterfacePPP::start()
{
    odebug << "InterfacePPP::start" << oendl; 

    if (data()->password().isEmpty() && !data()->storedUsername().isEmpty() ) {

        QDialog mb( 0, "Dialog", true );
        mb.setCaption( tr( "No password" ) );
        QVBoxLayout layout( &mb );
        QLabel text ( &mb  );
        text.setText( tr("Username defined but no password\n Please enter a password") );
        QLineEdit lineedit( &mb );
        lineedit.setEchoMode( QLineEdit::Password );
        layout.addWidget( &text );
        layout.addWidget( &lineedit );
        if ( mb.exec() == QDialog::Accepted )  {
            data()->setPassword( lineedit.text() );
        }
    }

  QFileInfo info(pppdPath());

  if(!info.exists()){
    QMessageBox::warning(0, tr("Error"),
                         QObject::tr("<qt>Cannot find the PPP daemon!<br>"
                              "Make sure that pppd is installed and "
                              "that you have entered the correct path.</qt>"));
    return;
  }
//#if 0
  if(!info.isExecutable()){

    QString string;
    string = QObject::tr( "<qt>Cannot execute:<br> %1<br>"
    		   "Please make sure that you have given "
		   "setuid permission and that "
		   "pppd is executable.<br>").arg(pppdPath());
    QMessageBox::warning(0, tr("Error"), string);
    return;

  }
//#endif

  QFileInfo info2(data()->modemDevice());

  if(!info2.exists()){
    QString string;
    string = QObject::tr( "<qt>Cannot find:<br> %1<br>"
                   "Please make sure you have setup "
		   "your modem device properly "
		   "and/or adjust the location of the modem device on "
		   "the modem tab of "
		   "the setup dialog.</qt>").arg(data()->modemDevice());
    QMessageBox::warning(0, tr("Error"), string);
    return;
  }

  // if this is a PAP or CHAP account, ensure that username is
  // supplied
  if(data()->authMethod() == AUTH_PAP ||
     data()->authMethod() == AUTH_CHAP ||
     data()->authMethod() == AUTH_PAPCHAP ) {
      if(false){ //FIXME: ID_Edit->text().isEmpty()) {
        QMessageBox::warning(0,tr("Error"),
			   QObject::tr("<qt>You have selected the authentication method PAP or CHAP. This requires that you supply a username and a password!</qt>"));
// FIXME:      return;
    } else {
      if(!modem()->setSecret(data()->authMethod(),
                             PPPData::encodeWord(data()->storedUsername()),
                             PPPData::encodeWord(data()->password()))
          ) {
	QString s;
	s = QObject::tr("<qt>Cannot create PAP/CHAP authentication<br>"
				     "file \"%1\"</qt>").arg(PAP_AUTH_FILE);
	QMessageBox::warning(0, tr("Error"), s);
	return;
      }
    }
  }

  if (data()->phonenumber().isEmpty()) {
    QString s = QObject::tr("You must specify a telephone number!");
    QMessageBox::warning(0, tr("Error"), s);
    return;
  }

  // SEGFAULTS:
//   setStatus( true );
//   emit updateInterface((Interface*) this);

  emit begin_connect();

  odebug << "InterfacePPP::start END" << oendl; 
}

void InterfacePPP::stop()
{
    odebug << "InterfacePPP::stop" << oendl; 
    // emit hangup_now();
    status = false; // not connected
    setStatus( false );
    emit hangup_now();
    refresh();

}

void InterfacePPP::save()
{
    data()->save();
    emit updateInterface((Interface*) this);
}
QString InterfacePPP::pppDev()const {
    return modem()->pppDevice();
}
pid_t InterfacePPP::pppPID()const{
    return modem()->pppPID();
}
void InterfacePPP::setPPPDpid( pid_t pid) {
    setStatus( true );
    modem()->setPPPDPid( pid );
}
