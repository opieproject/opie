TEMPLATE  = app
CONFIG    += qt warn_on quick-app
HEADERS   = advancedfm.h filePermissions.h output.h
SOURCES   = advancedfm.cpp advancedfmData.cpp advancedfmMenu.cpp filePermissions.cpp output.cpp main.cpp
TARGET    = advancedfm
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lopieui2

!contains(CONFIG,quick-app) {
 DESTDIR = $(OPIEDIR)/bin
 DEFINES += NOQUICKLAUNCH
}

include ( $(OPIEDIR)/include.pro )
