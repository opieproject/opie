CONFIG          = qt warn_on
DESTDIR         = $(OPIEDIR)/bin
HEADERS         = opiebluetoothd.h bluetoothagent.h bluetoothpindlg.h
SOURCES         = main.cpp opiebluetoothd.cpp bluetoothagent.cpp bluetoothpindlg.cpp
INTERFACES      = bluetoothpindlgbase.ui
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -lopiebluez2 -ldbus-qt2 $$system(pkg-config --libs dbus-1)

TARGET 		= opiebluetoothd

include( $(OPIEDIR)/include.pro )

