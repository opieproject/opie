TEMPLATE        = app
CONFIG          = qt warn_on
HEADERS         =
SOURCES         = onotifydemo.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lopiecore2 -lopieui2
TARGET          = onotifydemo

!contains( platform, x11 ) {
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
}

MOC_DIR = moc
