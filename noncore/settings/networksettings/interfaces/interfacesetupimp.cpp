#include "interfacesetupimp.h"
#include "interface.h"
#include "interfaces.h"

#include <qdialog.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qgroupbox.h>
#include <qlabel.h>

#include <qmessagebox.h>

#include <assert.h>

#define DNSSCRIPT "changedns"

/**
 * Constuctor.  Set up the connection and load the first profile.
 */ 
InterfaceSetupImp::InterfaceSetupImp(QWidget* parent, const char* name, Interface *i, WFlags fl) : InterfaceSetup(parent, name, fl){
  assert(parent);
  assert(i);
  interface = i;
  interfaces = new Interfaces();
  bool error = false;
  if(interfaces->getInterfaceMethod(error) == INTERFACES_LOOPBACK){
    staticGroupBox->hide();
    dhcpCheckBox->hide();
    leaseTime->hide();
    leaseHoursLabel->hide();
  }
}

/**
 * Save the current settings, then write out the interfaces file and close. 
 */
void InterfaceSetupImp::saveChanges(){
  if(!saveSettings())
    return;
  interfaces->write();
}

/**
 * Save the settings for the current Interface.
 * @return bool true if successfull, false otherwise
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
  
  if(!dhcpCheckBox->isChecked() && (ipAddressEdit->text().isEmpty() || subnetMaskEdit->text().isEmpty() || firstDNSLineEdit->text().isEmpty())){
   QMessageBox::information(this, "Empy Fields.", "Please fill in address, subnet,\n gateway and the first dns entries.", "Ok");
   return false;
  }	
  interfaces->removeAllInterfaceOptions();
  
  // DHCP
  if(dhcpCheckBox->isChecked()){
    interfaces->setInterfaceMethod(INTERFACES_METHOD_DHCP);
    interfaces->setInterfaceOption("leasehours", QString("%1").arg(leaseTime->value()));
    interfaces->setInterfaceOption("leasetime", QString("%1").arg(leaseTime->value()*60*60));
  }
  else{
    interfaces->setInterfaceMethod("static");
    interfaces->setInterfaceOption("address", ipAddressEdit->text());
    interfaces->setInterfaceOption("netmask", subnetMaskEdit->text());
    interfaces->setInterfaceOption("gateway", gatewayEdit->text());
    QString dns = firstDNSLineEdit->text() + " " + secondDNSLineEdit->text();
    interfaces->setInterfaceOption("up "DNSSCRIPT" -a ", dns);
    interfaces->setInterfaceOption("down "DNSSCRIPT" -r ", dns);
  }
  
  // IP Information
  interfaces->setAuto(interface->getInterfaceName(), autoStart->isChecked());
  return true;
}

/**
 * The Profile has changed.
 * @profile the new profile.
 */ 
void InterfaceSetupImp::setProfile(const QString &profile){
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
	  qDebug("InterfaceSetupImp: Added Mapping, but still can't set.");
          return;
	}
      }
      interfaces->setMap("map", newInterfaceName);
      interfaces->setScript("getprofile.sh");
    }
    else{
      interfaces->addInterface(newInterfaceName, INTERFACES_FAMILY_INET, INTERFACES_METHOD_DHCP);
      if(!interfaces->setInterface(newInterfaceName)){
        qDebug("InterfaceSetupImp: Added interface, but still can't set.");
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
  leaseTime->setValue(interfaces->getInterfaceOption("leasehours", error).toInt());
  if(error)
    leaseTime->setValue(interfaces->getInterfaceOption("leasetime", error).toInt()/60/60);
  if(error)
    leaseTime->setValue(24);

  // IP Information
  autoStart->setChecked(interfaces->isAuto(interface->getInterfaceName()));
  QString dns = interfaces->getInterfaceOption("up interfacednsscript -a", error);
  if(dns.contains(" ")){
    firstDNSLineEdit->setText(dns.mid(0, dns.find(" ")));
    secondDNSLineEdit->setText(dns.mid(dns.find(" ")+1, dns.length())); 
  }
  ipAddressEdit->setText(interfaces->getInterfaceOption("address", error));
  subnetMaskEdit->setText(interfaces->getInterfaceOption("netmask", error));
  gatewayEdit->setText(interfaces->getInterfaceOption("gateway", error)); 
}

  
// interfacesetup.cpp

