#ifndef WLAN_DATA_H
#define WLAN_DATA_H

#include <qstring.h>
typedef struct WLanData {
      QString ESSID;
      QString NodeName;
      short Mode;
      bool SpecificAP;
      QString APMac;
      bool Encrypted;
      QString Key[4];
      bool AcceptNonEncrypted;
} WLanData_t; 

#endif
