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

TRANSLATIONS = ../../../../i18n/de/libbluetoothapplet.ts \
	 ../../../../i18n/da/libbluetoothapplet.ts \
	 ../../../../i18n/xx/libbluetoothapplet.ts \
	 ../../../../i18n/en/libbluetoothapplet.ts \
	 ../../../../i18n/es/libbluetoothapplet.ts \
	 ../../../../i18n/fr/libbluetoothapplet.ts \
	 ../../../../i18n/hu/libbluetoothapplet.ts \
	 ../../../../i18n/ja/libbluetoothapplet.ts \
	 ../../../../i18n/ko/libbluetoothapplet.ts \
	 ../../../../i18n/no/libbluetoothapplet.ts \
	 ../../../../i18n/pl/libbluetoothapplet.ts \
	 ../../../../i18n/pt/libbluetoothapplet.ts \
	 ../../../../i18n/pt_BR/libbluetoothapplet.ts \
	 ../../../../i18n/sl/libbluetoothapplet.ts \
	 ../../../../i18n/zh_CN/libbluetoothapplet.ts \
	 ../../../../i18n/zh_TW/libbluetoothapplet.ts
