#ifndef BLUETOOTHRFCOMM_DATA_H
#define BLUETOOTHRFCOMM_DATA_H

#include <qstring.h>

typedef struct BluetoothRFCOMMData {
      QString Device;
      QString LockFile;
      long Speed;
      short Parity;
      short DataBits;
      short StopBits;
      bool HardwareControl;
      bool SoftwareControl;

} BluetoothRFCOMMData_t; 

#endif
