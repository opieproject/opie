#ifndef WLANIMP_H
#define WLANIMP_H

#include "wlan.h"

class InterfaceSetupImp;
class Interface;
class Config;

class WLANImp : public WLAN { 
  Q_OBJECT

public:
  WLANImp( QWidget* parent = 0, const char* name = 0, Interface *i=0, bool modal = FALSE, WFlags fl = 0 );
  ~WLANImp( );

protected:
  void accept();

private:
  void readConfig();
  bool writeConfig();
  bool writeWirelessOpts( QString scheme = "*" );
  bool writeWlanngOpts( QString scheme = "*" );
  Config* config;
  InterfaceSetupImp *interfaceSetup;

};

#endif
 
