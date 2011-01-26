CONFIG          = qt warn_on
DESTDIR         = $(OPIEDIR)/bin
HEADERS         = opiebluetoothd.h
SOURCES         = main.cpp opiebluetoothd.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -ldbus-qt2 $$system(pkg-config --libs dbus-1)

TARGET 		= opiebluetoothd

include( $(OPIEDIR)/include.pro )

