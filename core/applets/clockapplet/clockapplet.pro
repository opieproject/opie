TEMPLATE	= lib
CONFIG		+= qt plugin warn_on release
HEADERS	=   clock.h clockappletimpl.h
SOURCES	=   clock.cpp clockappletimpl.cpp
TARGET		= clockapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ..
LIBS            += -lqpe
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
