TEMPLATE  = lib
CONFIG    += qt plugin warn_on release
HEADERS =   vmemo.h vmemoimpl.h adpcm.h
SOURCES =   vmemo.cpp vmemoimpl.cpp adpcm.c
TARGET    = vmemoapplet
DESTDIR   =$(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe
VERSION   = 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
