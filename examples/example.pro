TEMPLATE	= lib
CONFIG		+= qt plugin warn_on release
HEADERS	=   exampleboardimpl.h
SOURCES	=   exampleboardimpl.cpp
TARGET		= example_board
DESTDIR		= $(OPIEDIR)/plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ../../launcher
LIBS            += -lqpe
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
