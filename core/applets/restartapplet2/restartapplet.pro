TEMPLATE  = lib
CONFIG    += qt plugin warn_on 
HEADERS =   restart.h
SOURCES =   restart.cpp
TARGET    = restartapplet2
DESTDIR   = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
VERSION   = 1.0.0
MOC_DIR=opieobj
OBJECTS_DIR=opieobj



include( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
