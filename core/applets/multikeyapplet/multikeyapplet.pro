TEMPLATE	= lib
CONFIG		+= qt plugin warn_on release
HEADERS	=   multikey.h multikeyappletimpl.h
SOURCES	=   multikey.cpp multikeyappletimpl.cpp
TARGET		= multikeyapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ..
LIBS            += -lqpe -lopie
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
