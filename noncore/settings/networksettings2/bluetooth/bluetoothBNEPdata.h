#ifndef BLUETOOTHBNEP_DATA_H
#define BLUETOOTHBNEP_DATA_H

#include <qstringlist.h>

typedef struct BluetoothBNEPData {
        bool        AllowAll;
        QStringList BDAddress;
} BluetoothBNEPData_t; 

#endif
