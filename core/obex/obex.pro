TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   obex.h obeximpl.h obexhandler.h obexsend.h receiver.h
SOURCES	=   obex.cc obeximpl.cpp obexsend.cpp obexhandler.cpp receiver.cpp
TARGET		= opieobex
DESTDIR		= $(OPIEDIR)/plugins/obex
INCLUDEPATH += $(OPIEDIR)/include $(OPIEDIR)/core/launcher
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe -lopie
VERSION		= 0.0.2

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
