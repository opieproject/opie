#ifndef WEXTENSIONS_H
#define WEXTENSIONS_H

#include <qstring.h>

#include <netinet/ip.h>
#include <linux/wireless.h>

class WExtensions {

public:
  WExtensions(QString interfaceName);
  QString getInterfaceName(){return interface;};
  bool doesHaveWirelessExtensions(){return hasWirelessExtensions;};
  QString station();
  QString essid();
  QString mode();
  double frequency();
  int channel();
  double rate();
  QString ap();
  bool stats( int &signal, int &noise, int &quality);

private:
  bool hasWirelessExtensions;
  QString interface;
  
  // Used in we calls
  struct iwreq iwr;
  int fd;

};

#endif
