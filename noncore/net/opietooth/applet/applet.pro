TEMPLATE      =  lib
CONFIG        += qt warn_on release
HEADERS       =  bluezapplet.h bluezappletimpl.h
SOURCES       =  bluezapplet.cpp bluezappletimpl.cpp
TARGET        =  bluetoothapplet
DESTDIR       =  $(OPIEDIR)/plugins/applets
INCLUDEPATH   += $(OPIEDIR)/include
INCLUDEPATH   += $(OPIEDIR)/noncore/net/opietooth/lib
DEPENDPATH    += $(OPIEDIR)/include
LIBS          += -lqpe -lopietooth
VERSION       = 0.0.3



TRANSLATIONS = ../../../../i18n/de/libbluetoothapplet.ts
TRANSLATIONS += ../../../../i18n/en/libbluetoothapplet.ts
TRANSLATIONS += ../../../../i18n/es/libbluetoothapplet.ts
TRANSLATIONS += ../../../../i18n/fr/libbluetoothapplet.ts
TRANSLATIONS += ../../../../i18n/hu/libbluetoothapplet.ts
TRANSLATIONS += ../../../../i18n/ja/libbluetoothapplet.ts
TRANSLATIONS += ../../../../i18n/ko/libbluetoothapplet.ts
TRANSLATIONS += ../../../../i18n/no/libbluetoothapplet.ts
TRANSLATIONS += ../../../../i18n/pl/libbluetoothapplet.ts
TRANSLATIONS += ../../../../i18n/pt/libbluetoothapplet.ts
TRANSLATIONS += ../../../../i18n/pt_BR/libbluetoothapplet.ts
TRANSLATIONS += ../../../../i18n/sl/libbluetoothapplet.ts
TRANSLATIONS += ../../../../i18n/zh_CN/libbluetoothapplet.ts
TRANSLATIONS += ../../../../i18n/zh_TW/libbluetoothapplet.ts

