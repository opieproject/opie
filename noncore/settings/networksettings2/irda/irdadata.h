#ifndef IRDA_DATA_H
#define IRDA_DATA_H

#include <qstring.h>

class IRDAData {
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
