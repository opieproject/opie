TEMPLATE	= lib
CONFIG		+= qt warn_on 
HEADERS	=   obex.h obexhandler.h obexsend.h receiver.h obeximpl.h
SOURCES	=   obex.cc obexsend.cpp obexhandler.cpp receiver.cpp obeximpl.cpp
TARGET		= opieobex
DESTDIR		= $(OPIEDIR)/plugins/obex
INCLUDEPATH += $(OPIEDIR)/include $(OPIEDIR)/core/launcher
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
VERSION		= 0.0.2

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
