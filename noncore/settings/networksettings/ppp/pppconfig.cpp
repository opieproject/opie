
#include <qlayout.h>
#include <qmessagebox.h>
#include <qtabwidget.h>

#include "accounts.h"
#include "devices.h"
#include "general.h"
#include "interfaceppp.h"
#include "modem.h"
#include "pppconfig.h"
#include "pppdata.h"
#include "runtests.h"

PPPConfigWidget::PPPConfigWidget( InterfacePPP* iface, QWidget *parent,
                                  const char *name,
                                  bool modal, WFlags fl )
  : QDialog(parent, name, modal, fl)
{
    setCaption(tr("Configure Modem"));
    int result = runTests();
    if(result == TEST_CRITICAL){
        QMessageBox::critical(0, tr("Modem failure"), tr("A critical failure appeard while testing the modem") );
        return;
    }

    interface = iface;
    odebug << "PPPConfigWidget::PPPConfigWidget" << oendl; 
    odebug << " interface->getHardwareName >" << interface->getHardwareName().latin1() << "<" << oendl; 

    odebug << " _pppdata->accname >" << interface->data()->accname().latin1() << "<" << oendl; 


  QVBoxLayout *layout = new QVBoxLayout( this );
  layout->setSpacing( 0 );
  layout->setMargin( 1 );
  tabWindow = new QTabWidget( this, "tabWidget" );
  layout->addWidget( tabWindow );

  accounts = new AccountWidget( interface->data(), tabWindow, "accounts", Qt::WStyle_ContextHelp  );
  tabWindow->addTab( accounts, tr("&Accounts") );
  devices = new DevicesWidget( interface, tabWindow, "devices", Qt::WStyle_ContextHelp );
  tabWindow->addTab( devices, tr("&Devices") );

//OLD:
//   modem1 = new ModemWidget( interface, tabWindow, "modem1" );
//   tabWindow->addTab( modem1, tr("&Device") );
//   modem2 = new ModemWidget2( interface, tabWindow, "modem2" );
//   tabWindow->addTab( modem2, tr("&Modem") );

}


PPPConfigWidget::~PPPConfigWidget()
{

}

void PPPConfigWidget::accept()
{
    odebug << "PPPConfigWidget::accept" << oendl; 
    odebug << " _pppdata->accname >" << interface->data()->accname().latin1() << "<" << oendl; 
    odebug << " interface->getHardwareName >" << interface->getHardwareName().latin1() << "<" << oendl; 
    interface->setInterfaceName( interface->data()->devname() );
    interface->setHardwareName( interface->data()->accname() );
    interface->save();
    QDialog::accept();
}


void PPPConfigWidget::reject()
{
    interface->data()->cancel();
    QDialog::reject();
}
