#ifndef INTERFACEINFORMATIONPPP_H 
#define INTERFACEINFORMATIONPPP_H

#include "interfaceinformationimp.h"
//#include "interface.h"

class QWidget;
class Interface;

class InterfaceInformationPPP : public InterfaceInformationImp { 

Q_OBJECT
	
public:

  InterfaceInformationPPP(QWidget *parent=0, const char *name=0, Interface *i=0, WFlags f=0);
  ~InterfaceInformationPPP(){};
  
  
};

#endif

// addserviceimp.h

