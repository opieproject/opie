#ifndef WLANIMP_H
#define WLANIMP_H

#include "wlan.h"

#include <qpe/config.h>

class WLANImp : public WLAN { 
  Q_OBJECT

public:
  WLANImp( QWidget* parent = 0, const char* name = 0);

protected:
  void accept();
  void done ( int r );

private:
  void readConfig();
  bool writeConfig();
  bool writeWirelessOpts( QString scheme = "*" );
  bool writeWlanngOpts( QString scheme = "*" );
  Config* config;
};

#endif
 
