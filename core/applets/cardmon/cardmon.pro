TEMPLATE	= lib
CONFIG		+= qt plugin warn_on 
HEADERS	=	cardmon.h
SOURCES	=	cardmon.cpp
TARGET		= cardmonapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include ../launcher
LIBS            += -lqpe -lopiecore2
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
