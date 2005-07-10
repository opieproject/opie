TEMPLATE    = lib
CONFIG      += qt warn_on
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = obluetooth.h \
              oinfrared.h
SOURCES     = obluetooth.cpp \
              oinfrared.cpp
INTERFACES  =
TARGET      = opieshower2
VERSION     = 0.0.0
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lopiecore2 -lbluetooth

!contains( platform, x11 ) {
  include( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
}

!isEmpty( LIBBLUEZ_INC_DIR ) {
    INCLUDEPATH = $$LIBBLUEZ_INC_DIR $$INCLUDEPATH
}
!isEmpty( LIBBLUEZ_LIB_DIR ) {
    LIBS = -L$$LIBBLUEZ_LIB_DIR $$LIBS
}

