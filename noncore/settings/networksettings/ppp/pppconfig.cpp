
#include <qlayout.h>
#include <qmessagebox.h>
#include <qtabwidget.h>

#include "accounts.h"
#include "general.h"
#include "interface.h"
#include "modem.h"
#include "pppconfig.h"
#include "pppdata.h"
#include "runtests.h"

PPPConfigWidget::PPPConfigWidget( Interface* iface, QWidget *parent,
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
    if (!PPPData::data()->setModemDevice( interface->getInterfaceName() ))
        PPPData::data()->setModemDevice("/dev/modem");
    qDebug("PPPConfigWidget::PPPConfigWidget");
    qDebug(" interface->getHardwareName >%s<", interface->getHardwareName().latin1());
    if (!PPPData::data()->setAccount( interface->getHardwareName() ))
        PPPData::data()->setAccount( 0 );

    qDebug(" PPPData::data()->accname >%s<",PPPData::data()->accname().latin1());
    qDebug(" PPPData::data()->currentAccountID() >%i<",PPPData::data()->currentAccountID());
    (void)new Modem;

  QVBoxLayout *layout = new QVBoxLayout( this );
  layout->setSpacing( 0 );
  layout->setMargin( 1 );
  tabWindow = new QTabWidget( this, "tabWidget" );
  layout->addWidget( tabWindow );

  accounts = new AccountWidget( tabWindow, "accounts" );
  tabWindow->addTab( accounts, tr("&Accounts") );
  modem1 = new ModemWidget( tabWindow, "modem1" );
  tabWindow->addTab( modem1, tr("&Device") );
  modem2 = new ModemWidget2( tabWindow, "modem2" );
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
    qDebug(" PPPData::data()->accname >%s<",PPPData::data()->accname().latin1());
    qDebug(" interface->getHardwareName >%s<", interface->getHardwareName().latin1());
    interface->setInterfaceName( PPPData::data()->modemDevice() );
    interface->setHardwareName( PPPData::data()->accname() );
    PPPData::data()->save();
    QDialog::accept();
}


void PPPConfigWidget::reject()
{
    PPPData::data()->cancel();
    QDialog::reject();
}
