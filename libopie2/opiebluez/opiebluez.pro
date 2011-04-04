TEMPLATE    = lib
CONFIG      += qt warn_on
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = obluetooth.h
SOURCES     = obluetooth.cpp
INTERFACES  =
TARGET      = opiebluez2
VERSION     = 0.0.0
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lopiecore2 -ldbus-qt2 $$system(pkg-config --libs dbus-1)

!contains( platform, x11 ) {
  include( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
}

!isEmpty( LIBBLUEZ_INC_DIR ) {
    INCLUDEPATH = $$LIBBLUEZ_INC_DIR $$INCLUDEPATH
}
