#include "interfaceinformationimp.h"
#include "interfaceadvanced.h"

#include <qpushbutton.h>
#include <qlabel.h>
#include <assert.h>

/**
 * Constructor for the InterfaceInformationImp class.  This class pretty much
 * just display's information about the interface that is passed to it.
 */ 
InterfaceInformationImp::InterfaceInformationImp(QWidget *parent, const char *name, Interface *i, WFlags f):InterfaceInformation(parent, name, f){
  assert(i);
  
  interface = i;
  connect(i, SIGNAL(updateInterface(Interface *)), this, SLOT(updateInterface(Interface *)));
  updateInterface(interface);
  connect(startButton, SIGNAL(clicked()), interface, SLOT(start()));
  connect(stopButton, SIGNAL(clicked()), interface, SLOT(stop()));
  connect(restartButton, SIGNAL(clicked()), interface, SLOT(restart()));
  connect(refreshButton, SIGNAL(clicked()), interface, SLOT(refresh()));
  connect(advancedButton, SIGNAL(clicked()), this, SLOT(advanced()));
  
}

/**
 * Update the interface information and buttons.
 * @param Intarface *i the interface to update (should be the one we already
 *                     know about).
 */ 
void InterfaceInformationImp::updateInterface(Interface *i){
  if(interface->getStatus()){
    startButton->setEnabled(false);
    stopButton->setEnabled(true);
    restartButton->setEnabled(true);
  }
  else{
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
    restartButton->setEnabled(false);
  }
  macAddressLabel->setText(interface->getMacAddress());
  ipAddressLabel->setText(interface->getIp());
  subnetMaskLabel->setText(interface->getSubnetMask());
  broadcastLabel->setText(interface->getBroadcast());
}

/**
 * Create the advanced widget. Fill it with the current interface's information.
 * Display it.
 */ 
void InterfaceInformationImp::advanced(){
  InterfaceAdvanced *a = new InterfaceAdvanced(0, "InterfaceAdvanced");
  a->interfaceName->setText(interface->getInterfaceName());
  a->macAddressLabel->setText(interface->getMacAddress());
  a->ipAddressLabel->setText(interface->getIp());
  a->subnetMaskLabel->setText(interface->getSubnetMask());
  a->broadcastLabel->setText(interface->getBroadcast());
  a->dhcpServerLabel->setText(interface->getDhcpServerIp());
  a->leaseObtainedLabel->setText(interface->getLeaseObtained());
  a->leaseExpiresLabel->setText(interface->getLeaseExpires());
  a->showMaximized();
  a->show();
}

// infoimp.cpp

