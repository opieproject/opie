TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   kprocess.h kprocctrl.h obex.h
SOURCES	=   kprocess.cpp kprocctrl.cpp obex.cc
TARGET		= obex
DESTDIR		= ../../plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe
VERSION		= 0.0.1

