TEMPLATE        = app
CONFIG          = qt warn_on 
HEADERS         =
SOURCES         = odebugdemo.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lopiecore2
TARGET          = odebugdemo

!contains( platform, x11 ) {
  include( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib 
}

