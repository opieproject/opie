#ifndef CABLE_DATA_H
#define CABLE_DATA_H

#include <qstring.h>

typedef struct CableData {
      QString Device;
      QString LockFile;
      long Speed;
      short Parity;
      short DataBits;
      short StopBits;
      bool HardwareControl;
      bool SoftwareControl;

} CableData_t; 

#endif
