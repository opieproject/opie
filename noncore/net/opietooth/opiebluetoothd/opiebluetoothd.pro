CONFIG          = qt warn_on
DESTDIR         = $(OPIEDIR)/bin
HEADERS         = opiebluetoothd.h bluetoothagent.h bluetoothpindlg.h hciattach.h rfkill.h
SOURCES         = main.cpp opiebluetoothd.cpp bluetoothagent.cpp bluetoothpindlg.cpp hciattach.cpp rfkill.cpp
INTERFACES      = bluetoothpindlgbase.ui
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -lopiebluez2 -lopieui2 -ldbus-qt2 $$system(pkg-config --libs dbus-1) -lsysfs

TARGET 		= opiebluetoothd

include( $(OPIEDIR)/include.pro )

