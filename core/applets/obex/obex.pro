TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   obex.h obeximpl.h
SOURCES	=   obex.cc obeximpl.cc
TARGET		= opieobex
DESTDIR		= ../../plugins/obex
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include
INTERFACES      = obexinc.ui obexdlg.ui
LIBS            += -lqpe -lopie
VERSION		= 0.0.1

