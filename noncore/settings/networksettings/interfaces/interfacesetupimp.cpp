#include "interfacesetupimp.h"
#include "interface.h"

#include <qcheckbox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qgroupbox.h>
#include <qlabel.h>

#include <qmessagebox.h>

#include <opie2/oprocess.h>

#ifdef QWS
#include <opie2/owait.h>
#include <qpe/global.h>
#include <qapplication.h>
#endif

#define DNSSCRIPT "changedns"

/**
 * Constuctor.  Set up the connection. A profile must be set.
 */
using namespace Opie::Ui;
using namespace Opie::Core;
InterfaceSetupImp::InterfaceSetupImp(QWidget* parent, const char* name, Interface *i, Interfaces *j, WFlags fl) : InterfaceSetup(parent, name, fl), interface(i), interfaces(j), delInterfaces(false){
  if (j == 0) {
    delInterfaces = true;
    interfaces = new Interfaces;
  }
}

/**
 * Destructor
 */
InterfaceSetupImp::~InterfaceSetupImp(){
  if(delInterfaces) {
    delete interfaces;
  }
}

/**
 * Save the current settings, then write out the interfaces file and close.
 */
bool InterfaceSetupImp::saveChanges(){
  bool error;
  QString iface = interfaces->getInterfaceName(error);
  qDebug("InterfaceSetupImp::saveChanges saves interface %s", iface.latin1() );
  if(!saveSettings())
    return false;

  interfaces->write();

  if (interface->getStatus()) {
    QString ifup;
    ifup += "ifdown ";
    ifup += iface;
    ifup += "; ifup ";
    ifup += iface;
    ifup += ";";

    OProcess restart;
    restart << "sh";
    restart << "-c";
    restart << ifup;

    OWait *owait = new OWait();
    Global::statusMessage( tr( "Restarting interface" ) );

    owait->show();
    qApp->processEvents();

    if (!restart.start(OProcess::Block, OProcess::NoCommunication) ) {
      qWarning("unstable to spawn ifdown/ifup");
    }

    owait->hide();
    delete owait;

    interface->refresh();
  }
  return true;
}

/**
 * Save the settings for the current Interface.
 * @return bool true if successful, false otherwise
 */
bool InterfaceSetupImp::saveSettings(){
  // eh can't really do anything about it other then return. :-D
  if(!interfaces->isInterfaceSet())
    return true;

  bool error = false;
  // Loopback case
  if(interfaces->getInterfaceMethod(error) == INTERFACES_LOOPBACK){
    interfaces->setAuto(interface->getInterfaceName(), autoStart->isChecked());
    return true;
  }

  if(!dhcpCheckBox->isChecked() && (ipAddressEdit->text().isEmpty() || subnetMaskEdit->text().isEmpty())){
   QMessageBox::information(this, "Not Saved.", "Please fill in the IP address and\n subnet entries.", QMessageBox::Ok);
   return false;
  }
  // DHCP
  if(dhcpCheckBox->isChecked()) {
    interfaces->setInterfaceMethod(INTERFACES_METHOD_DHCP);
    interfaces->removeInterfaceOption("address");
    interfaces->removeInterfaceOption("netmask");
    interfaces->removeInterfaceOption("gateway");
    interfaces->removeInterfaceOption("up "DNSSCRIPT" -a ");
    interfaces->removeInterfaceOption("down "DNSSCRIPT" -r ");
  } else{
    interfaces->setInterfaceMethod("static");
    interfaces->setInterfaceOption("address", ipAddressEdit->text());
    interfaces->setInterfaceOption("netmask", subnetMaskEdit->text());
    interfaces->setInterfaceOption("gateway", gatewayEdit->text());
    if(!firstDNSLineEdit->text().isEmpty() || !secondDNSLineEdit->text().isEmpty()){
      QString dns = firstDNSLineEdit->text() + " " + secondDNSLineEdit->text();
      interfaces->setInterfaceOption("up "DNSSCRIPT" -a ", dns);
      interfaces->setInterfaceOption("down "DNSSCRIPT" -r ", dns);
    }else{
        interfaces->removeInterfaceOption("up "DNSSCRIPT" -a ");
        interfaces->removeInterfaceOption("down "DNSSCRIPT" -r");
    }
  }

  // IP Information
  interfaces->setAuto(interface->getInterfaceName(), autoStart->isChecked());
  return true;
}

/**
 * The Profile has changed.
 * @param QString profile the new profile.
 */
void InterfaceSetupImp::setProfile(const QString &profile){
   /*
  bool error = false;
  if(interfaces->getInterfaceMethod(error) == INTERFACES_LOOPBACK){
    staticGroupBox->hide();
    dhcpCheckBox->hide();
    leaseTime->hide();
    leaseHoursLabel->hide();
  }
  */

  QString newInterfaceName = interface->getInterfaceName();
  if(profile.length() > 0)
    newInterfaceName += "_" + profile;
  // See if we have to make a interface.
  if(!interfaces->setInterface(newInterfaceName)){
    // Add making for this new interface if need too
    if(profile != ""){
      interfaces->copyInterface(interface->getInterfaceName(), newInterfaceName);
      if(!interfaces->setMapping(interface->getInterfaceName())){
        interfaces->addMapping(interface->getInterfaceName());
        if(!interfaces->setMapping(interface->getInterfaceName())){
      qDebug("InterfaceSetupImp: Added Mapping, but still can't setInterface.");
          return;
    }
      }
      interfaces->setMap("map", newInterfaceName);
      interfaces->setScript("getprofile.sh");
    }
    else{
      interfaces->addInterface(newInterfaceName, INTERFACES_FAMILY_INET, INTERFACES_METHOD_DHCP);
      if(!interfaces->setInterface(newInterfaceName)){
        qDebug("InterfaceSetupImp: Added interface, but still can't setInterface.");
        return;
      }
    }
  }

  // We must have a valid interface to get this far so read some settings.

  // DHCP
  bool error = false;
  if(interfaces->getInterfaceMethod(error) == INTERFACES_METHOD_DHCP)
    dhcpCheckBox->setChecked(true);
  else
    dhcpCheckBox->setChecked(false);

  // IP Information
  autoStart->setChecked(interfaces->isAuto(interface->getInterfaceName()));
  QString dns = interfaces->getInterfaceOption("up "DNSSCRIPT" -a", error);
  qDebug("dns >%s<",dns.latin1());
  if(dns.contains(" ")){
    firstDNSLineEdit->setText(dns.mid(0, dns.find(" ")));
    secondDNSLineEdit->setText(dns.mid(dns.find(" ")+1, dns.length()));
  }else firstDNSLineEdit->setText(dns);

  ipAddressEdit->setText(interfaces->getInterfaceOption("address", error));
  subnetMaskEdit->setText(interfaces->getInterfaceOption("netmask", error));
  if (subnetMaskEdit->text().isEmpty())
      subnetMaskEdit->setText( "255.255.255.0" );
  gatewayEdit->setText(interfaces->getInterfaceOption("gateway", error));



  qWarning("InterfaceSetupImp::setProfile(%s)\n", profile.latin1());
  qWarning("InterfaceSetupImp::setProfile: iface is %s\n", interfaces->getInterfaceName(error).latin1());

}

// interfacesetup.cpp

