#ifndef MODEM_DATA_H
#define MODEM_DATA_H

#include <qstring.h>
class ModemData {
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
