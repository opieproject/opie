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
  updateInterface();
  connect(startButton, SIGNAL(clicked()), this, SLOT(start()));
  connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));
  connect(restartButton, SIGNAL(clicked()), this, SLOT(restart()));
  connect(refreshButton, SIGNAL(clicked()), this, SLOT(refresh()));
  connect(advancedButton, SIGNAL(clicked()), this, SLOT(advanced()));
  
}

void InterfaceInformationImp::updateInterface(){
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
 * Start the interface. Update the information if successfull
 */ 
void InterfaceInformationImp::start(){
  if(interface->start()){
    updateInterface();
  }
}

/**
 * Stop the interface.
 */ 
void InterfaceInformationImp::stop(){
  if(interface->stop()){
    updateInterface();
  }
}

/***
 * Tell the interface to refresh its information.
 **/
void InterfaceInformationImp::refresh(){
  if(interface->refresh())
    updateInterface();
}

void InterfaceInformationImp::restart(){
  if(interface->restart()){
     updateInterface();
  }
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

