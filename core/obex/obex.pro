TEMPLATE	= lib
CONFIG		+= qt warn_on 
HEADERS	=   obex.h btobex.h obexhandler.h obexsend.h receiver.h obeximpl.h
SOURCES	=   obex.cpp btobex.cpp obexsend.cpp obexhandler.cpp receiver.cpp obeximpl.cpp
TARGET		= opieobex
DESTDIR		= $(OPIEDIR)/plugins/obex
INTERFACES  = obexsendbase.ui
INCLUDEPATH += $(OPIEDIR)/include $(OPIEDIR)/core/launcher $(OPIEDIR)/noncore/net/opietooth/lib
DEPENDPATH  += 
LIBS        += -lopietooth1 -lqpe -lopiecore2 
VERSION		= 0.0.3

include( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
