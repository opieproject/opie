#ifndef WLANIMP_H
#define WLANIMP_H

#include "wlan.h"

class Config;

class WLANImp : public WLAN { 
  Q_OBJECT

public:
  WLANImp( Config& cfg, QWidget* parent = 0, const char* name = 0);

protected:
  void accept();
  void done ( int r );

private:
  void readConfig();
  bool writeConfig();
  bool writeWirelessOpts( Config &cfg, QString scheme = "*" );
  bool writeWlanngOpts( Config &cfg, QString scheme = "*" );
  Config& config;
};

#endif
 
