TEMPLATE  = app
CONFIG    += qt warn_on release
HEADERS   = advancedfm.h filePermissions.h output.h
SOURCES   = advancedfm.cpp advancedfmData.cpp advancedfmMenu.cpp filePermissions.cpp output.cpp main.cpp
TARGET    = advancedfm
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
DESTDIR = $(OPIEDIR)/bin
LIBS            += -lqpe -lopie

include ( $(OPIEDIR)/include.pro )
