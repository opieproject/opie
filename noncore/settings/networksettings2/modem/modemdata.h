#ifndef MODEM_DATA_H
#define MODEM_DATA_H

#include <qstring.h>
typedef struct ModemData {
      QString Device;
      QString LockFile;
      long Speed;
      short Parity;
      short DataBits;
      short StopBits;
      bool HardwareControl;
      bool SoftwareControl;

} ModemData_t; 

#endif
