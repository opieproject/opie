TEMPLATE	= lib
CONFIG		+= qt plugin warn_on 
HEADERS	=   multikey.h
SOURCES	=   multikey.cpp
TARGET		= multikeyapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      +=  ..
LIBS            += -lqpe
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
