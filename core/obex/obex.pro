TEMPLATE	= lib
CONFIG		+= qt warn_on 
HEADERS		= btobex.h obexhandler.h receiver.h obeximpl.h
SOURCES		= btobex.cpp obexhandler.cpp receiver.cpp obeximpl.cpp
TARGET		= opieobex
DESTDIR		= $(OPIEDIR)/plugins/obex
INTERFACES  = obexsendbase.ui
INCLUDEPATH += $(OPIEDIR)/include $(OPIEDIR)/core/launcher
LIBS		+= -lqpe -lopiecore2
VERSION		= 0.0.3

include( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
