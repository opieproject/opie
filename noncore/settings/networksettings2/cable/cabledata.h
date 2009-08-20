#ifndef CABLE_DATA_H
#define CABLE_DATA_H

#include <qstring.h>

class CableData {

public :

      QString Device;
      QString LockFile;
      long Speed;
      short Parity;
      short DataBits;
      short StopBits;
      bool HardwareControl;
      bool SoftwareControl;

};

#endif
