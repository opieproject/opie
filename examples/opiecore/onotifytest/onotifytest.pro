TEMPLATE        = app
CONFIG          = qt warn_on
HEADERS         = main.h
SOURCES         = main.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lopiecore2
TARGET          = onotifytest

!contains( platform, x11 ) {
  include( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
}

