
#include <qmessagebox.h>
#define i18n QObject::tr

#include "auth.h"
#include "interfaceppp.h"
#include "modem.h"
#include "pppdata.h"

InterfacePPP::InterfacePPP(QObject *parent, const char *name, bool status)
    : Interface(parent, name, status),
      _modemPtr(0),
      _dataPtr(0)
{
    qDebug("InterfacePPP::InterfacePPP(");
}

PPPData* InterfacePPP::data()
{
    if (!_dataPtr){
        qDebug("creating new Data obj");
        _dataPtr = new PPPData();
        _dataPtr->setModemDevice( getInterfaceName() );
        _dataPtr->setAccount( getHardwareName() );
    }
    return _dataPtr;
}

Modem* InterfacePPP::modem()
{
    if (!_modemPtr){
        qDebug("creating new modem obj");
        _modemPtr = new Modem( data() );
    }
    return _modemPtr;
}

bool InterfacePPP::refresh()
{
    qDebug("InterfacePPP::refresh()");
    QMessageBox::information(0,"Not Implemented","This feature is not yet implemneted... ;-(");
    return false;
}

void InterfacePPP::start()
{
    qDebug("InterfacePPP::start");
// should work...
//     if (data()->password().isEmpty() ){
// //FIXME: ask for password
//         qDebug("using dummy password");
//         QMessageBox::critical( 0, "no password", "you should be prompted for a password, but you are not! ;-)");
//     }


  QFileInfo info(pppdPath());

  if(!info.exists()){
    QMessageBox::warning(0, tr("Error"),
                         i18n("<qt>Cannot find the PPP daemon!<br>"
                              "Make sure that pppd is installed and "
                              "that you have entered the correct path.</qt>"));
    return;
  }
//#if 0
  if(!info.isExecutable()){

    QString string;
    string = i18n( "<qt>Cannot execute:<br> %1<br>"
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
    string = i18n( "<qt>Cannot find:<br> %1<br>"
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
			   i18n("<qt>You have selected the authentication method PAP or CHAP. This requires that you supply a username and a password!</qt>"));
// FIXME:      return;
    } else {
      if(!modem()->setSecret(data()->authMethod(),
                             PPPData::encodeWord(data()->storedUsername()),
                             PPPData::encodeWord(data()->password()))
          ) {
	QString s;
	s = i18n("<qt>Cannot create PAP/CHAP authentication<br>"
				     "file \"%1\"</qt>").arg(PAP_AUTH_FILE);
	QMessageBox::warning(0, tr("Error"), s);
	return;
      }
    }
  }

  if (data()->phonenumber().isEmpty()) {
    QString s = i18n("You must specify a telephone number!");
    QMessageBox::warning(0, tr("Error"), s);
    return;
  }

  // SEGFAULTS:
//   setStatus( true );
//   emit updateInterface((Interface*) this);

  emit begin_connect();

  qDebug("InterfacePPP::start END");
}

void InterfacePPP::stop()
{
    qDebug("InterfacePPP::stop");

}

void InterfacePPP::save()
{
    data()->save();
    emit updateInterface((Interface*) this);
}
