TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   obex.h obexhandler.h obexsend.h receiver.h
SOURCES	=   obex.cc obexsend.cpp obexhandler.cpp receiver.cpp
TARGET		= opieobex
DESTDIR		= $(OPIEDIR)/plugins/obex
INCLUDEPATH += $(OPIEDIR)/include $(OPIEDIR)/core/launcher
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
VERSION		= 0.0.2

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
