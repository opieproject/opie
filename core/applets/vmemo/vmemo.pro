TEMPLATE  = lib
CONFIG    += qt plugin warn_on 
HEADERS =   vmemo.h
SOURCES =   vmemo.cpp
TARGET    = vmemoapplet
DESTDIR   =$(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopiemm2
VERSION   = 1.0.0

include( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
