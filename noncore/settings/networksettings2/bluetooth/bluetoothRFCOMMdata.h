#ifndef BLUETOOTHRFCOMM_DATA_H
#define BLUETOOTHRFCOMM_DATA_H

#include <qstring.h>
#include <qvector.h>

class RFCOMMChannel {

public :
      QString BDAddress;
      QString Name;
      int     Channel;
};

class BluetoothRFCOMMData {

public :

      QVector<RFCOMMChannel> Devices;
};

#endif
