#ifndef VPN_DATA_H
#define VPN_DATA_H

#include <qstring.h>
class VPNData {
public :
      QString Device;
      QString LockFile;
      long Speed;
      short Parity;
      short DataBits;
      short StopBits;
      bool HardwareControl;
      bool SoftwareControl;
} ; 

#endif
