#ifndef INTERFACEINFORMATIONIMP_H
#define INTERFACEINFORMATIONIMP_H

#include "interfaceinformation.h"
#include "interface.h"

/**
 * This widget just shows some generic information about the
 * given Interface. IP Addresses and such will be shown.
 */
class InterfaceInformationImp : public InterfaceInformation {

Q_OBJECT

public:
  InterfaceInformationImp(QWidget *parent=0, const char *name=0, Interface *i=0, WFlags f=0);
  ~InterfaceInformationImp();

private slots:
  void advanced();
  void updateInterface(Interface *i);
  void showMessage(const QString &message);

private:
  Interface *interface;

};

#endif

// addserviceimp.h

