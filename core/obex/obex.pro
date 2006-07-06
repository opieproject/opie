TEMPLATE	= lib
CONFIG		+= qt warn_on 
HEADERS	=   obex.h btobex.h obexhandler.h obexsend.h receiver.h obeximpl.h obexbase.h obexserver.h
SOURCES	=   obex.cpp btobex.cpp obexsend.cpp obexhandler.cpp receiver.cpp obeximpl.cpp obexbase.cpp obexserver.cpp
TARGET		= opieobex
DESTDIR		= $(OPIEDIR)/plugins/obex
INTERFACES  = obexsendbase.ui
INCLUDEPATH += $(OPIEDIR)/include $(OPIEDIR)/core/launcher 
DEPENDPATH  += 
LIBS        += -lqpe -lopiecore2 
VERSION		= 0.0.4

include( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets

#FIXME: These parameters are used if bluetooth is used
INCLUDEPATH += $(OPIEDIR)/noncore/net/opietooth/lib
LIBS        += -lopietooth1 -lbluetooth -lopenobex
DEFINES     += BLUETOOTH

