#ifndef INTERFACESETUPIMP_H
#define INTERFACESETUPIMP_H

#include "interfacesetup.h"

class Interface;
class Interfaces;

class InterfaceSetupImp : public InterfaceSetup {
  Q_OBJECT
	
public:
  InterfaceSetupImp( QWidget* parent = 0, const char* name = 0, Interface *i=0, bool modal = FALSE, WFlags fl = 0);

protected slots:
  void accept();

public slots:
  void setProfile(const QString &profile);

private:
  bool saveSettings();
  Interfaces *interfaces;
  Interface *interface;

};

#endif

// interfacesetupimp.h

