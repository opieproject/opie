#ifndef WLAN_DATA_H
#define WLAN_DATA_H

#include <qstring.h>
typedef struct WLanData {
      QString Device;
      QString LockFile;
      long Speed;
      short Parity;
      short DataBits;
      short StopBits;
      bool HardwareControl;
      bool SoftwareControl;

} WLanData_t; 

#endif
