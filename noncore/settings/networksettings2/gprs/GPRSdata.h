#ifndef GPRS_DATA_H
#define GPRS_DATA_H

#include <qvector.h>
class GPRSRoutingEntry {
public :
      QString Address;
      short   Mask;
};

class GPRSData {
public :
      QString                APN;
      QString                User;
      QString                Password;
      QString                DNS1;
      QString                DNS2;
      bool                   SetIfSet;
      bool                   DefaultGateway;
      short                  Debug;
      QVector<GPRSRoutingEntry> Routing;
} ; 

#endif
