TEMPLATE	= lib
#CONFIG		= qt plugin warn_on debug
CONFIG		= qt plugin warn_on release
HEADERS		= keyview.h keyboardimpl.h
SOURCES		= keyview.cpp keyboardimpl.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include ../../launcher
LIBS            += -lqpe
INTERFACES	= 
TARGET		= keyview
DESTDIR     = $(OPIEDIR)/plugins/inputmethods

include ( $(OPIEDIR)/include.pro )
