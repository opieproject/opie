#ifndef INTERFACEINFORMATIONIMP_H 
#define INTERFACEINFORMATIONIMP_H

#include "interfaceinformation.h"
#include "interface.h"

class InterfaceInformationImp : public InterfaceInformation {

Q_OBJECT
	
public:
  InterfaceInformationImp(QWidget *parent=0, const char *name=0, Interface *i=0, WFlags f=0);
  ~InterfaceInformationImp(){};
  
private slots:
  void start();
  void stop();
  void refresh();
  void restart();
  void advanced();
  Interface *interface;

private:
  void updateInterface();
  
};

#endif

// addserviceimp.h

