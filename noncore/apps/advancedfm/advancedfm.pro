TEMPLATE  = app
CONFIG    += qt warn_on release
HEADERS   = advancedfm.h inputDialog.h filePermissions.h output.h
SOURCES   = advancedfm.cpp inputDialog.cpp filePermissions.cpp output.cpp main.cpp
TARGET    = advancedfm
REQUIRES=medium-config
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
DESTDIR = $(OPIEDIR)/bin
LIBS            += -lqpe -lopie

