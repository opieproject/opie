#ifndef NETWORK_DATA_H
#define NETWORK_DATA_H

#include <qstring.h>
#include <qstringlist.h>
typedef struct NetworkData {
      bool UseDHCP;
      QString Hostname;
      bool SendHostname;
      QString IPAddress;
      QString NetMask;
      QString Gateway;
      QString Broadcast;
      QString DNS1;
      QString DNS2;
      QStringList PreUp_SL;
      QStringList PostUp_SL;
      QStringList PreDown_SL;
      QStringList PostDown_SL;
} NetworkData_t; 

#endif
