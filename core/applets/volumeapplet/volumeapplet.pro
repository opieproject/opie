TEMPLATE	= lib
CONFIG		+= qt plugin warn_on release
HEADERS	=   volume.h volumeappletimpl.h oledbox.h
SOURCES	=   volume.cpp volumeappletimpl.cpp oledbox.cpp
TARGET		= volumeapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
