
#include <qlayout.h>
#include <qmessagebox.h>
#include <qtabwidget.h>

#include "accounts.h"
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
    qDebug("PPPConfigWidget::PPPConfigWidget");
    qDebug(" interface->getHardwareName >%s<", interface->getHardwareName().latin1());

    qDebug(" _pppdata->accname >%s<",interface->data()->accname().latin1());
    qDebug(" _pppdata->currentAccountID() >%i<",interface->data()->currentAccountID());

  QVBoxLayout *layout = new QVBoxLayout( this );
  layout->setSpacing( 0 );
  layout->setMargin( 1 );
  tabWindow = new QTabWidget( this, "tabWidget" );
  layout->addWidget( tabWindow );

  accounts = new AccountWidget( interface->data(), tabWindow, "accounts" );
  tabWindow->addTab( accounts, tr("&Accounts") );
  modem1 = new ModemWidget( interface, tabWindow, "modem1" );
  tabWindow->addTab( modem1, tr("&Device") );
  modem2 = new ModemWidget2( interface, tabWindow, "modem2" );
  tabWindow->addTab( modem2, tr("&Modem") );
//    graph = new GraphSetup( tabWindow->addPage( tr("&Graph"), tr("Throughput Graph" ) ) );
//    general = new GeneralWidget( tabWindow->addPage( tr("M&isc"), tr("Miscellaneous Settings") ) );

}


PPPConfigWidget::~PPPConfigWidget()
{

}

void PPPConfigWidget::accept()
{
    qDebug("PPPConfigWidget::accept");
    qDebug(" _pppdata->accname >%s<",interface->data()->accname().latin1());
    qDebug(" interface->getHardwareName >%s<", interface->getHardwareName().latin1());
    interface->setInterfaceName( interface->data()->modemDevice() );
    interface->setHardwareName( interface->data()->accname() );
    interface->data()->save();
    QDialog::accept();
}


void PPPConfigWidget::reject()
{
    interface->data()->cancel();
    QDialog::reject();
}
