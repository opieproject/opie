#ifndef WLANIMP_H
#define WLANIMP_H

#include "wlan.h"
#include "interfaces.h"
#include <qstringlist.h>

class InterfaceSetupImp;
class Interface;
class Config;

class WLANImp : public WLAN { 
  Q_OBJECT

public:
  WLANImp( QWidget* parent = 0, const char* name = 0, Interface *i=0, bool modal = FALSE, WFlags fl = 0 );
  ~WLANImp();
  void setProfile(const QString &profile);

protected:
  void accept();

private:
  void parseOpts();
  void writeOpts();

  void parseKeyStr(QString keystr);
  
  InterfaceSetupImp *interfaceSetup;
  Interfaces *interfaces;
  Interface  *interface;

  QString currentProfile;
};

#endif
 
