TEMPLATE  = lib
CONFIG    += qt plugin warn_on 
HEADERS =   restart.h restartappletimpl.h
SOURCES =   restart.cpp restartappletimpl.cpp
TARGET    = restartapplet
DESTDIR   = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += 
LIBS            += -lqpe
VERSION   = 1.0.0
MOC_DIR=opieobj
OBJECTS_DIR=opieobj



include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
