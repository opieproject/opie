TEMPLATE	= lib
CONFIG		+= qt plugin warn_on release
HEADERS		= volume.h oledbox.h
SOURCES		= volume.cpp oledbox.cpp
TARGET		= volumeapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
