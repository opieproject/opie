TEMPLATE	= lib
CONFIG		+= qt warn_on 
HEADERS	=   obex.h obexhandler.h obexsend.h receiver.h obeximpl.h obexbase.h obexserver.h
SOURCES	=   obex.cpp obexsend.cpp obexhandler.cpp receiver.cpp obeximpl.cpp obexbase.cpp obexserver.cpp
TARGET		= opieobex
DESTDIR		= $(OPIEDIR)/plugins/obex
INTERFACES  = obexsendbase.ui
INCLUDEPATH += $(OPIEDIR)/include $(OPIEDIR)/core/launcher 
DEPENDPATH  += 
LIBS        += -lqpe -lopiecore2 -lopenobex
VERSION		= 0.0.4

include( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets

#FIXME: These parameters are used if bluetooth is used
CONFTEST = $$system( echo $CONFIG_LIBOPIETOOTH )
contains( CONFTEST, y ){
HEADERS     += btobex.h
SOURCES     += btobex.cpp
INCLUDEPATH += $(OPIEDIR)/noncore/net/opietooth/lib
LIBS        += -lopietooth1 -lbluetooth
DEFINES     += BLUETOOTH
}
