TEMPLATE	= lib
CONFIG		+= qt plugin warn_on release
HEADERS	=   clipboard.h clipboardappletimpl.h
SOURCES	=   clipboard.cpp clipboardappletimpl.cpp
TARGET		= clipboardapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
